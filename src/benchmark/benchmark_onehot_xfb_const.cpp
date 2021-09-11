#include "benchmark_onehot_xfb_const.h"

#include <chrono>
#include <random>
#include <span>

#include <config.h>
#include <logger.h>
#include <shader.h>

bool benchmark::benchmark_onehot_xfb_const::checkSupport() noexcept {
	return benchmark_onehot_xfb::checkSupport();
}

benchmark::benchmark_onehot_xfb_const::benchmark_onehot_xfb_const() {
	gl::GLuint vertShader, fragShader;

	vertShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "benchmark_xfb_v_const");
	fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, "dummy_f");

	program_xfb = common::createProgram({vertShader, fragShader}, "outVert");

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);
}

std::pair<double, benchmark::benchmark_base::doRMSE_return_t> benchmark::benchmark_onehot_xfb_const::drawFrame() {
	generatePoints();
	clearResults();

	gl::glEnable(gl::GL_RASTERIZER_DISCARD);

	gl::glBindVertexArray(vao);
	gl::glUseProgram(program_xfb);
	gl::glBindBufferBase(gl::GL_TRANSFORM_FEEDBACK_BUFFER, 0, xfb_buffer);

	gl::glBeginTransformFeedback(gl::GL_POINTS);

	const auto start = std::chrono::high_resolution_clock::now();

	gl::glDrawArrays(gl::GL_POINTS, 0, common::samples);
	gl::glEndTransformFeedback();

	gl::glGetNamedBufferSubData(xfb_buffer, 0, std::span(points_result).size_bytes(), points_result.data());

	const auto end = std::chrono::high_resolution_clock::now();

	if constexpr (common::isDebug) {
		common::graphics_logger()->debug("Buffer[0] {}", points_result[0]);
		common::graphics_logger()->debug("Buffer[1] {}", points_result[1]);
		common::graphics_logger()->debug("Buffer[2] {}", points_result[2]);
		common::graphics_logger()->debug("Buffer[3] {}", points_result[3]);
		common::graphics_logger()->debug("Buffer[4] {}", points_result[4]);
		common::graphics_logger()->debug("Buffer[5] {}", points_result[5]);
		common::graphics_logger()->debug("Buffer[6] {}", points_result[6]);
	}

	gl::glDisable(gl::GL_RASTERIZER_DISCARD);

	return std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6, doRMSE());
}
