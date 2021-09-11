#include "benchmark_track_xfb_const.h"

#include <chrono>
#include <random>
#include <span>

#include <config.h>
#include <logger.h>
#include <shader.h>

#include <spdlog/stopwatch.h>

bool benchmark::benchmark_track_xfb_const::checkSupport() noexcept {
	return benchmark_track_xfb::checkSupport() && glsl::checkSupport();
}

benchmark::benchmark_track_xfb_const::benchmark_track_xfb_const() {
	gl::GLuint vertShader, geomShader, fragShader;

	vertShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "benchmark_xfb_v_passthrough");
	geomShader = common::loadCompileShader(gl::GL_GEOMETRY_SHADER, "benchmark_xfb_g_const", nullptr);
	fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, "dummy_f");

	program_xfb = common::createProgram({vertShader, geomShader, fragShader}, "outVert");

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(geomShader);
	gl::glDeleteShader(fragShader);
}

std::pair<double, benchmark::benchmark_base::doRMSE_return_t> benchmark::benchmark_track_xfb_const::drawFrame() {
	generatePoints();
	clearResults();

	gl::glEnable(gl::GL_RASTERIZER_DISCARD);

	gl::glBindVertexArray(vao);
	gl::glUseProgram(program_xfb);
	gl::glBindBufferBase(gl::GL_TRANSFORM_FEEDBACK_BUFFER, 0, xfb_buffer);

	gl::glBeginTransformFeedback(gl::GL_POINTS);

	const auto start = std::chrono::high_resolution_clock::now();

	gl::glDrawArrays(gl::GL_POINTS, 0, common::tracks);
	gl::glEndTransformFeedback();

	gl::glGetNamedBufferSubData(xfb_buffer, 0, std::span(points_result).size_bytes(), points_result.data());

	const auto end = std::chrono::high_resolution_clock::now();

	if constexpr (common::isDebug) {
		for (std::size_t i = 0; i < common::tracks; ++i) {
			common::graphics_logger()->debug("Buffer[{}] {}", i, points_result[i * common::pointsPerTrack]);
		}
	}

	gl::glDisable(gl::GL_RASTERIZER_DISCARD);

	return std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6, doRMSE());
}
