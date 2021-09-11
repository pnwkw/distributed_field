#include "benchmark_track_xfb_glsl_cache.h"

#include <chrono>
#include <random>
#include <span>

#include <config.h>
#include <logger.h>
#include <shader.h>

#include <spdlog/stopwatch.h>

bool benchmark::benchmark_track_xfb_glsl_cache::checkSupport() noexcept {
	return benchmark_track_xfb::checkSupport() && glsl::checkSupport();
}

benchmark::benchmark_track_xfb_glsl_cache::benchmark_track_xfb_glsl_cache() {
	gl::GLuint vertShader, geomShader, fragShader;

	vertShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "benchmark_xfb_v_passthrough");
	geomShader = common::loadCompileShader(gl::GL_GEOMETRY_SHADER, "benchmark_xfb_g_glsl_cache", nullptr);
	fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, "dummy_f");

	program_xfb = common::createProgram({vertShader, geomShader, fragShader}, "outVert");

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(geomShader);
	gl::glDeleteShader(fragShader);

	glslHandler = std::make_unique<decltype(glslHandler)::element_type>();

	gl::glCreateBuffers(1, &cache_ssbo);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_BUFFER, cache_ssbo, -1, "cache_ssbo");
	}
}

benchmark::benchmark_track_xfb_glsl_cache::~benchmark_track_xfb_glsl_cache() {
	gl::glDeleteBuffers(1, &cache_ssbo);
}

std::pair<double, benchmark::benchmark_base::doRMSE_return_t> benchmark::benchmark_track_xfb_glsl_cache::drawFrame() {
	generatePoints();
	clearResults();

	const auto cache_buff = std::make_unique<Cache>();

	std::fill(std::begin(cache_buff->SolSegCache), std::end(cache_buff->SolSegCache), -1);
	std::fill(std::begin(cache_buff->DipSegCache), std::end(cache_buff->DipSegCache), -1);

	gl::glNamedBufferData(cache_ssbo, sizeof(Cache), cache_buff.get(), gl::GL_DYNAMIC_DRAW);

	gl::glEnable(gl::GL_RASTERIZER_DISCARD);

	gl::glBindVertexArray(vao);
	gl::glUseProgram(program_xfb);
	gl::glBindBufferBase(gl::GL_TRANSFORM_FEEDBACK_BUFFER, 0, xfb_buffer);
	gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 1, glslHandler->getSolSegmentsBufferName());
	gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 2, glslHandler->getDipSegmentsBufferName());
	gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 3, glslHandler->getSolParamsBufferName());
	gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 4, glslHandler->getDipParamsBufferName());
	gl::glBindBufferBase(gl::GL_SHADER_STORAGE_BUFFER, 5, cache_ssbo);

	gl::glBeginTransformFeedback(gl::GL_POINTS);

	const auto start = std::chrono::high_resolution_clock::now();

	gl::glDrawArrays(gl::GL_POINTS, 0, common::tracks);
	gl::glEndTransformFeedback();

	gl::glGetNamedBufferSubData(xfb_buffer, 0, std::span(points_result).size_bytes(), points_result.data());

	const auto end = std::chrono::high_resolution_clock::now();

	if constexpr (common::isDebug) {
		auto max_dbg_points = std::min<std::size_t>(common::tracks, 5);
		for (std::size_t t = 0; t < max_dbg_points; t++) {
			common::graphics_logger()->debug("DBG <---->");
			for (std::size_t p = 0; p < 3; p++) {
				common::graphics_logger()->debug("Buffer[{},{}] {}", t, p, points_result[t * common::pointsPerTrack + p]);
			}
		}

		const auto ret_buff = std::make_unique<Cache>();
		gl::glGetNamedBufferSubData(cache_ssbo, 0, sizeof(Cache), ret_buff.get());
		for (std::size_t t = 0; t < max_dbg_points; t++) {
			common::graphics_logger()->debug("Sol Buffer[{}] {}", t, ret_buff->SolSegCache[t]);
			common::graphics_logger()->debug("Dip Buffer[{}] {}", t, ret_buff->DipSegCache[t]);
		}
	}

	gl::glDisable(gl::GL_RASTERIZER_DISCARD);

	return std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6, doRMSE(true));
}
