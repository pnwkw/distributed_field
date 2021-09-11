#include <debug_render_texture.h>

#include <glm/gtc/matrix_transform.hpp>
#include <shader.h>
#include <logger.h>
#include <bitmap_image.h>
#include <span>

benchmark::debug_render_texture::debug_render_texture() {
	gl::glCreateFramebuffers(1, &framebuffer);

	gl::glCreateTextures(gl::GL_TEXTURE_2D, 1, &framebuffer_texture);
	gl::glObjectLabel(gl::GL_TEXTURE, framebuffer_texture, -1, "Dbg Framebuffer");

	gl::glTextureStorage2D(framebuffer_texture, 1, gl::GL_RGB32F, mag_field::detector_dimensions.z, mag_field::detector_dimensions.x);

	gl::glNamedFramebufferTexture(framebuffer, gl::GL_COLOR_ATTACHMENT0, framebuffer_texture, 0);

	gl::GLenum drawBuffer = gl::GL_COLOR_ATTACHMENT0;
	gl::glNamedFramebufferDrawBuffers(framebuffer, 1, &drawBuffer);

	if (gl::glCheckNamedFramebufferStatus(framebuffer, gl::GL_FRAMEBUFFER) != gl::GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer incomplete!");
	}

	gl::glCreateVertexArrays(1, &vao);
	gl::glObjectLabel(gl::GL_VERTEX_ARRAY, vao, -1, "Dbg VAO");

	gl::glCreateBuffers(1, &vertex_buffer);

	gl::glEnableVertexArrayAttrib(vao, debug_render_texture::VTX_BINDING);

	gl::glEnableVertexArrayAttrib(vao, Point::POS_ATTRIB);
	gl::glVertexArrayAttribFormat(vao, Point::POS_ATTRIB, decltype(Point::pos)::length(), gl::GL_FLOAT, gl::GL_FALSE, offsetof(Point, pos));

	gl::glVertexArrayVertexBuffer(vao, debug_render_texture::VTX_BINDING, vertex_buffer, 0, sizeof(Point));
	gl::glVertexArrayAttribBinding(vao, Point::POS_ATTRIB, debug_render_texture::VTX_BINDING);

	std::vector<decltype(Point::pos)> points = {
			{mag_field::detector_min.z, mag_field::detector_min.x},
			{mag_field::detector_min.z, mag_field::detector_max.x},
			{mag_field::detector_max.z, mag_field::detector_min.x},
			{mag_field::detector_max.z, mag_field::detector_max.x},
	};

	gl::glNamedBufferData(vertex_buffer, points.size() * sizeof(decltype(points)::value_type), points.data(), gl::GL_STATIC_DRAW);

	const auto left = static_cast<float>(mag_field::detector_min.z);
	const auto right = static_cast<float>(mag_field::detector_max.z);
	const auto bottom = static_cast<float>(mag_field::detector_min.x);
	const auto top = static_cast<float>(mag_field::detector_max.x);

	const glm::mat4 projectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	const glm::mat4 viewMatrix = glm::mat4(1.0f);
	const glm::mat4 modelMatrix = glm::mat4(1.0f);

	ubo_data.MVP = projectionMatrix * viewMatrix * modelMatrix;

	gl::glCreateBuffers(1, &ubo_buffer);
	gl::glObjectLabel(gl::GL_BUFFER, ubo_buffer, -1, "Dbg UBO Buffer");

	gl::glNamedBufferData(this->ubo_buffer, sizeof(Ubo), &ubo_data, gl::GL_STATIC_DRAW);

	gl::GLuint vertShader, fragShader;

	vertShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "debug_v");
	fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, "debug_f");

	debug_program = common::createProgram({vertShader, fragShader});

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);
}

benchmark::debug_render_texture::~debug_render_texture() {
	gl::glDeleteProgram(debug_program);
	gl::glDeleteBuffers(1, &ubo_buffer);
	gl::glDeleteBuffers(1, &vertex_buffer);
	gl::glDeleteVertexArrays(1, &vao);
	gl::glDeleteTextures(1, &framebuffer_texture);
	gl::glDeleteFramebuffers(1, &framebuffer);
}

void benchmark::debug_render_texture::draw(std::function<void()> const &bindFunc) {
	gl::glGetIntegerv(gl::GL_VIEWPORT, viewport_values);

	gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, framebuffer);
	gl::glViewport(0, 0, mag_field::detector_dimensions.z, mag_field::detector_dimensions.x);
	gl::glClear(gl::GL_COLOR_BUFFER_BIT);

	gl::glUseProgram(debug_program);

	gl::glBindVertexArray(vao);
	gl::glBindBufferBase(gl::GL_UNIFORM_BUFFER, 0, ubo_buffer);

	bindFunc();

	gl::glDrawArrays(gl::GL_TRIANGLE_STRIP, 0, 4);

	gl::glViewport(viewport_values[0], viewport_values[1], viewport_values[2], viewport_values[3]);

	gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0);

	struct Pixel {
		std::uint8_t r;
		std::uint8_t g;
		std::uint8_t b;
	};

	std::vector<Pixel> pixels(mag_field::detector_dimensions.z * mag_field::detector_dimensions.x);

	gl::glPixelStorei(gl::GL_PACK_ALIGNMENT, 1);
	gl::glGetTextureImage(framebuffer_texture, 0, gl::GL_RGB, gl::GL_UNSIGNED_BYTE, std::span(pixels).size_bytes(), pixels.data());

	bitmap_image image(mag_field::detector_dimensions.z, mag_field::detector_dimensions.x);

	auto data = reinterpret_cast<uint8_t *>(pixels.data());

	for (std::size_t y = 0; y < image.height(); ++y) {
		for (std::size_t x = 0; x < image.width(); ++x) {
			const auto r = *(data++);
			const auto g = *(data++);
			const auto b = *(data++);

			image.set_pixel(x, y, r, g, b);
		}
	}

	image.save_image("dbg.bmp");
}
