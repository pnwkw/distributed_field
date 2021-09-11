#include "texture.h"

#include <config.h>
#include <logger.h>

bool benchmark::texture::checkSupport() noexcept {
	gl::GLint max_texture_size;
	gl::glGetIntegerv(gl::GL_MAX_3D_TEXTURE_SIZE, &max_texture_size);
	common::graphics_logger()->info("GL_MAX_3D_TEXTURE_SIZE {}", max_texture_size);

	const bool sizeAdequate = texture_dimensions_scaled.x <= max_texture_size && texture_dimensions_scaled.y <= max_texture_size && texture_dimensions_scaled.z <= max_texture_size;

	common::graphics_logger()->info("3D textures are {}big enough", sizeAdequate ? "" : "not ");

	return sizeAdequate;
}

benchmark::texture::texture() {
	gl::glCreateTextures(gl::GL_TEXTURE_3D, 1, &texture3d);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_TEXTURE, texture3d, -1, "Texture 3D");
	}

	gl::glTextureStorage3D(texture3d, 1, gl::GL_RGB32F, texture_dimensions_scaled.s, texture_dimensions_scaled.t, texture_dimensions_scaled.p);

	auto row_buff = std::vector<glm::vec3>(texture_dimensions_scaled.s);

	mag = std::make_unique<mag_field::mag_cheb>();

	common::graphics_logger()->info("Filling texture memory...");

	for (std::size_t p = 0; p < texture_dimensions_scaled.p; ++p) {
		for (std::size_t t = 0; t < texture_dimensions_scaled.t; ++t) {
			for (std::size_t s = 0; s < texture_dimensions_scaled.s; ++s) {
				const auto tex = glm::ivec3(s, t, p);
				const auto world = texToWorld(tex);
				const auto f = mag->Field(world);
				row_buff[s] = f;
			}
			gl::glTextureSubImage3D(texture3d, 0, 0, t, p, texture_dimensions_scaled.s, 1, 1, gl::GL_RGB, gl::GL_FLOAT, row_buff.data());
		}
	}
}

benchmark::texture::~texture() {
	gl::glDeleteTextures(1, &texture3d);
}

glm::ivec3 benchmark::texture::worldToTex(const glm::ivec3 &w) {
	auto r = glm::ivec3(w.z + mag_field::z_detector - mag_field::center.z, w.x + mag_field::x_detector - mag_field::center.x, w.y + mag_field::y_detector - mag_field::center.y);
	r = r / scales;
	return r;
}

glm::ivec3 benchmark::texture::texToWorld(const glm::ivec3 &t) {
	return glm::vec3(t.t * scales.t - mag_field::x_detector + mag_field::center.x, t.p * scales.p - mag_field::y_detector + mag_field::center.y, t.s * scales.s - mag_field::z_detector + mag_field::center.z);
}
