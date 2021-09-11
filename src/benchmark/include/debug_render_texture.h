#ifndef BENCHMARK_DEBUG_RENDER_TEXTURE_H
#define BENCHMARK_DEBUG_RENDER_TEXTURE_H

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>
#include <glm_helper.h>

#include <config.h>
#include <mag_cheb.h>
#include <measurements.h>

#include <functional>

namespace benchmark {
	class debug_render_texture {
	private:
		gl::GLuint framebuffer{};
		gl::GLuint framebuffer_texture{};

		gl::GLuint vao{};
		gl::GLuint vertex_buffer{};
		gl::GLuint ubo_buffer{};

		gl::GLuint debug_program{};

		constexpr static gl::GLuint VTX_BINDING = 0;

		struct Point {
			glm::vec2 pos;
			constexpr static gl::GLuint POS_ATTRIB = 0;
		};

		struct Ubo {
			glm::mat4 MVP;
		};

		Ubo ubo_data{};

		gl::GLint viewport_values[4]{};

	public:
		debug_render_texture();
		~debug_render_texture();

		void draw(std::function<void()> const &bindFunc);
	};
}// namespace benchmark

#endif//BENCHMARK_DEBUG_RENDER_TEXTURE_H
