#include "sparse.h"

#include <span>

#include <config.h>
#include <logger.h>

#include <glbinding-aux/ContextInfo.h>

constexpr int int_ceil(int x, int y) {
	return x / y + !!(x % y);
}

bool benchmark::sparse::checkSupport() noexcept {
	//WARNING: GL_ARB_sparse_texture2 guarantees reads from uncommited memory to be zero. On GL_ARB_sparse_texture it is undefined!
	const auto sparseSupported = glbinding::aux::ContextInfo::supported({gl::GLextension::GL_ARB_sparse_texture});
	const auto sparse2Supported = glbinding::aux::ContextInfo::supported({gl::GLextension::GL_ARB_sparse_texture2});
	const auto sparseAnySupported = sparseSupported || sparse2Supported;
	common::graphics_logger()->info("Sparse textures are {}supported", sparseAnySupported ? "" : "not ");

	gl::GLint max_texture_size;
	gl::glGetIntegerv(gl::GL_MAX_SPARSE_3D_TEXTURE_SIZE_ARB, &max_texture_size);
	common::graphics_logger()->info("GL_MAX_SPARSE_3D_TEXTURE_SIZE {}", max_texture_size);

	const auto tds = glm::ivec3(mag_field::detector_dimensions.z, mag_field::detector_dimensions.x, mag_field::detector_dimensions.y) / scales;

	const bool sizeAdequate = tds.x <= max_texture_size && tds.y <= max_texture_size && tds.z <= max_texture_size;

	common::graphics_logger()->info("Sparse textures are {}big enough", sizeAdequate ? "" : "not ");

	return sparseAnySupported && sizeAdequate;
}

benchmark::sparse::sparse() {
	gl::glCreateTextures(gl::GL_TEXTURE_3D, 1, &texture3d);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_TEXTURE, texture3d, -1, "Texture 3D");
	}

	gl::glTextureParameteri(texture3d, gl::GL_TEXTURE_SPARSE_ARB, gl::GL_TRUE);

	gl::GLint nSizes;

	gl::glGetInternalformativ(gl::GL_TEXTURE_3D, gl::GL_RGB32F, gl::GL_NUM_VIRTUAL_PAGE_SIZES_ARB, sizeof(gl::GLint), &nSizes);

	std::vector<gl::GLint> sizes_s(nSizes);
	std::vector<gl::GLint> sizes_t(nSizes);
	std::vector<gl::GLint> sizes_p(nSizes);

	gl::glGetInternalformativ(gl::GL_TEXTURE_3D, gl::GL_RGB32F, gl::GL_VIRTUAL_PAGE_SIZE_X_ARB, std::span(sizes_s).size_bytes(), sizes_s.data());
	gl::glGetInternalformativ(gl::GL_TEXTURE_3D, gl::GL_RGB32F, gl::GL_VIRTUAL_PAGE_SIZE_Y_ARB, std::span(sizes_t).size_bytes(), sizes_t.data());
	gl::glGetInternalformativ(gl::GL_TEXTURE_3D, gl::GL_RGB32F, gl::GL_VIRTUAL_PAGE_SIZE_Z_ARB, std::span(sizes_p).size_bytes(), sizes_p.data());

	sparse_page_sizes.s = sizes_s[0];
	sparse_page_sizes.t = sizes_t[0];
	sparse_page_sizes.p = sizes_p[0];

	texture_dimensions_scaled = glm::ivec3(mag_field::detector_dimensions.z, mag_field::detector_dimensions.x, mag_field::detector_dimensions.y) / scales;

	texture_dimensions_scaled.s = int_ceil(texture_dimensions_scaled.s, sparse_page_sizes.s) * sparse_page_sizes.s;
	texture_dimensions_scaled.t = int_ceil(texture_dimensions_scaled.t, sparse_page_sizes.t) * sparse_page_sizes.t;
	texture_dimensions_scaled.p = int_ceil(texture_dimensions_scaled.p, sparse_page_sizes.p) * sparse_page_sizes.p;

	gl::glTextureStorage3D(texture3d, 1, gl::GL_RGB32F, texture_dimensions_scaled.s, texture_dimensions_scaled.t, texture_dimensions_scaled.p);

	auto row_buff = std::vector<glm::vec3>(texture_dimensions_scaled.s);

	mag = std::make_unique<mag_field::mag_cheb>();

	const auto texSizeInPages = texture_dimensions_scaled / sparse_page_sizes;

	//Calculate regions to commit
	mag->appendDipPages(pages, worldToTex, texSizeInPages, sparse_page_sizes);
	mag->appendSolPages(pages, worldToTex, texSizeInPages, sparse_page_sizes);

	common::graphics_logger()->info("Allocating and filling texture memory...");

	for (auto const &page : pages) {
		gl::glTexturePageCommitmentEXT(texture3d, 0, page.s, page.t, page.p, sparse_page_sizes.s, sparse_page_sizes.t, sparse_page_sizes.p, true);

		for (std::size_t p = 0; p < sparse_page_sizes.p; ++p) {
			for (std::size_t t = 0; t < sparse_page_sizes.t; ++t) {
				for (std::size_t s = 0; s < sparse_page_sizes.s; ++s) {
					const auto tex = glm::ivec3(s + page.s, t + page.t, p + page.p);
					const auto world = texToWorld(tex);
					const auto f = mag->Field(world);
					row_buff[s] = f;
				}
				gl::glTextureSubImage3D(texture3d, 0, page.s, t + page.t, p + page.p, sparse_page_sizes.s, 1, 1, gl::GL_RGB, gl::GL_FLOAT, row_buff.data());
			}
		}
	}
}

benchmark::sparse::~sparse() {
	gl::glDeleteTextures(1, &texture3d);
}

glm::ivec3 benchmark::sparse::worldToTex(const glm::ivec3 &w) {
	auto r = glm::ivec3(w.z + mag_field::z_detector - mag_field::center.z, w.x + mag_field::x_detector - mag_field::center.x, w.y + mag_field::y_detector - mag_field::center.y);
	r = r / scales;
	return r;
}

glm::ivec3 benchmark::sparse::texToWorld(const glm::ivec3 &t) {
	return glm::vec3(t.t * scales.t - mag_field::x_detector + mag_field::center.x, t.p * scales.p - mag_field::y_detector + mag_field::center.y, t.s * scales.s - mag_field::z_detector + mag_field::center.z);
}

bool benchmark::sparse::dbgIsInside(glm::ivec3 const &w) {
	const auto alignPageBoundary = [&](glm::ivec3 const &w) {
		return (w / sparse_page_sizes) * sparse_page_sizes;
	};

	return pages.find(alignPageBoundary(w)) != pages.end();
}

void benchmark::sparse::dbgProjectionZX() {
	common::graphics_logger()->debug("Debug Projection ZX...");
	gl::GLuint texture2d_debug;

	gl::glCreateTextures(gl::GL_TEXTURE_2D, 1, &texture2d_debug);
	gl::glObjectLabel(gl::GL_TEXTURE, texture2d_debug, -1, "Debug Texture ZX");

	gl::glTextureStorage2D(texture2d_debug, 1, gl::GL_RGB32F, texture_dimensions_scaled.s, texture_dimensions_scaled.p);

	auto d2_row_buff = std::vector<glm::vec3>(texture_dimensions_scaled.s);

	const auto t = texture_dimensions_scaled.t / 2;

	for (int p = 0; p < texture_dimensions_scaled.p; ++p) {
		for (int s = 0; s < texture_dimensions_scaled.s; ++s) {
			const auto tex = glm::ivec3(s, t, p);
			const bool inside = dbgIsInside(tex);
			auto r = 0;
			if (inside) {
				r = 100;
			}
			const auto f = mag->Field(glm::vec3(texToWorld(tex)));
			auto g = glm::length(f) > 0 ? 100 : 0;
			d2_row_buff[s] = glm::vec3(r, g, 0);
		}
		gl::glTextureSubImage2D(texture2d_debug, 0, 0, p, texture_dimensions_scaled.s, 1, gl::GL_RGB, gl::GL_FLOAT, d2_row_buff.data());
	}

	gl::glBindTextureUnit(3, texture2d_debug);
}

void benchmark::sparse::dbgProjectionXY() {
	common::graphics_logger()->debug("Debug Projection XY...");
	gl::GLuint texture2d_debug;
	gl::glCreateTextures(gl::GL_TEXTURE_2D, 1, &texture2d_debug);
	gl::glObjectLabel(gl::GL_TEXTURE, texture2d_debug, -1, "Debug Texture XY");

	gl::glTextureStorage2D(texture2d_debug, 1, gl::GL_RGB32F, texture_dimensions_scaled.t, texture_dimensions_scaled.p);

	auto d2_row_buff = std::vector<glm::vec3>(texture_dimensions_scaled.t);

	const auto s = (texture_dimensions_scaled.s * 8) / 10;

	for (int p = 0; p < texture_dimensions_scaled.p; ++p) {
		for (int t = 0; t < texture_dimensions_scaled.t; ++t) {
			const auto tex = glm::ivec3(s, t, p);
			const bool inside = dbgIsInside(tex);
			auto r = 0;
			if (inside) {
				r = 100;
			}
			const auto f = mag->Field(glm::vec3(texToWorld(tex)));
			auto g = glm::length(f) > 0 ? 100 : 0;
			d2_row_buff[t] = glm::vec3(r, g, 0);
		}
		gl::glTextureSubImage2D(texture2d_debug, 0, 0, p, texture_dimensions_scaled.t, 1, gl::GL_RGB, gl::GL_FLOAT, d2_row_buff.data());
	}

	gl::glBindTextureUnit(4, texture2d_debug);
}

void benchmark::sparse::dbgProjectionZY() {
	common::graphics_logger()->debug("Debug Projection ZY...");
	gl::GLuint texture2d_debug;
	gl::glCreateTextures(gl::GL_TEXTURE_2D, 1, &texture2d_debug);
	gl::glObjectLabel(gl::GL_TEXTURE, texture2d_debug, -1, "Debug Texture ZY");

	gl::glTextureStorage2D(texture2d_debug, 1, gl::GL_RGB32F, texture_dimensions_scaled.s, texture_dimensions_scaled.t);

	auto d2_row_buff = std::vector<glm::vec3>(texture_dimensions_scaled.s);

	const auto p = texture_dimensions_scaled.p / 2;

	for (int t = 0; t < texture_dimensions_scaled.t; ++t) {
		for (int s = 0; s < texture_dimensions_scaled.s; ++s) {
			const auto tex = glm::ivec3(s, t, p);
			const bool inside = dbgIsInside(tex);
			auto r = 0;
			if (inside) {
				r = 100;
			}
			const auto f = mag->Field(glm::vec3(texToWorld(tex)));
			auto g = glm::length(f) > 0 ? 100 : 0;
			d2_row_buff[s] = glm::vec3(r, g, 0);
		}
		gl::glTextureSubImage2D(texture2d_debug, 0, 0, t, texture_dimensions_scaled.s, 1, gl::GL_RGB, gl::GL_FLOAT, d2_row_buff.data());
	}

	gl::glBindTextureUnit(5, texture2d_debug);
}
