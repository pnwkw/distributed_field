#include "benchmark_onehot_xfb_glsl_cache.h"

#include <chrono>
#include <random>
#include <span>

#include <config.h>
#include <logger.h>
#include <shader.h>

bool benchmark::benchmark_onehot_xfb_glsl_cache::checkSupport() noexcept {
	return benchmark_onehot_xfb::checkSupport() && glsl::checkSupport();
}

benchmark::benchmark_onehot_xfb_glsl_cache::benchmark_onehot_xfb_glsl_cache() {
	if (common::samples > common::maxSamples) {
		common::graphics_logger()->error("{} > {}", common::samples, common::maxSamples);
		throw std::runtime_error("Configured samples count exceed defined maximum");
	}

	gl::GLuint vertShader, fragShader;

	vertShader = common::loadCompileShader(gl::GL_VERTEX_SHADER, "benchmark_xfb_v_glsl_cache");
	fragShader = common::loadCompileShader(gl::GL_FRAGMENT_SHADER, "dummy_f");

	program_xfb = common::createProgram({vertShader, fragShader}, "outVert");

	gl::glDeleteShader(vertShader);
	gl::glDeleteShader(fragShader);

	glslHandler = std::make_unique<decltype(glslHandler)::element_type>();

	gl::glCreateBuffers(1, &cache_ssbo);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_BUFFER, cache_ssbo, -1, "cache_ssbo");
	}

	cache = std::make_unique<Cache>();
}

benchmark::benchmark_onehot_xfb_glsl_cache::~benchmark_onehot_xfb_glsl_cache() {
	gl::glDeleteBuffers(1, &cache_ssbo);
}

std::pair<double, benchmark::benchmark_base::doRMSE_return_t> benchmark::benchmark_onehot_xfb_glsl_cache::drawFrame() {
	generatePoints();
	clearResults();

	std::fill(std::begin(cache->SolSegCache), std::end(cache->SolSegCache), -1);
	std::fill(std::begin(cache->DipSegCache), std::end(cache->DipSegCache), -1);

	gl::glNamedBufferData(cache_ssbo, sizeof(Cache), cache.get(), gl::GL_DYNAMIC_DRAW);

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

	return std::make_pair(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6, doRMSE(true));
}
