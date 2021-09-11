#include "benchmark_onehot_xfb_tex_sparse.h"

#include <random>
#include <span>

#include <config.h>
#include <logger.h>
#include <shader.h>

bool benchmark::benchmark_onehot_xfb_tex_sparse::checkSupport() noexcept {
	return benchmark_onehot_xfb::checkSupport() && sparse::checkSupport();
}

benchmark::benchmark_onehot_xfb_tex_sparse::benchmark_onehot_xfb_tex_sparse() {
	gl::GLuint vertShader, fragShader;

	vertShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "benchmark_xfb_v_texture");
	fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, "dummy_f");

	program_xfb = common::createProgram({vertShader, fragShader}, "outVert");

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);

	textureHandler = std::make_unique<decltype(textureHandler)::element_type>();
}

std::pair<double, benchmark::benchmark_base::doRMSE_return_t> benchmark::benchmark_onehot_xfb_tex_sparse::drawFrame() {
	generatePoints();
	clearResults();

	gl::glEnable(gl::GL_RASTERIZER_DISCARD);

	gl::glBindVertexArray(vao);
	gl::glUseProgram(program_xfb);
	gl::glBindBufferBase(gl::GL_TRANSFORM_FEEDBACK_BUFFER, 0, xfb_buffer);
	gl::glBindTextureUnit(0, textureHandler->getTexture());

	gl::glBeginTransformFeedback(gl::GL_POINTS);

	const auto start = std::chrono::high_resolution_clock::now();

	gl::glDrawArrays(gl::GL_POINTS, 0, common::samples);
	gl::glEndTransformFeedback();

	gl::glGetNamedBufferSubData(xfb_buffer, 0, std::span(points_result).size_bytes(), points_result.data());

	const auto end = std::chrono::high_resolution_clock::now();

	if constexpr (common::isDebug) {
		spdlog::info("Buffer[0] {}", points_result[0]);
		spdlog::info("Buffer[1] {}", points_result[1]);
		spdlog::info("Buffer[2] {}", points_result[2]);
		spdlog::info("Buffer[3] {}", points_result[3]);
		spdlog::info("Buffer[4] {}", points_result[4]);
		spdlog::info("Buffer[5] {}", points_result[5]);
		spdlog::info("Buffer[6] {}", points_result[6]);
	}

	gl::glDisable(gl::GL_RASTERIZER_DISCARD);

	return std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6, doRMSE());
}
