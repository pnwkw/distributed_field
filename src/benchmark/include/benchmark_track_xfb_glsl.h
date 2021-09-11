#ifndef BENCHMARK_BENCHMARK_TRACK_XFB_GLSL_H
#define BENCHMARK_BENCHMARK_TRACK_XFB_GLSL_H

#include <memory>
#include <utility>

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>

#include "benchmark_track_xfb.h"
#include "glsl.h"

namespace benchmark {
	class benchmark_track_xfb_glsl final : public benchmark_track_xfb {
	private:
		std::unique_ptr<glsl> glslHandler;

	public:
		static bool checkSupport() noexcept;

		std::string getName() const noexcept final { return "benchmark_track_xfb_glsl"; };

		benchmark_track_xfb_glsl();

		std::pair<double, doRMSE_return_t> drawFrame() final;
	};
}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_TRACK_XFB_GLSL_H
