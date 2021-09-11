#ifndef BENCHMARK_BENCHMARK_TRACK_XFB_CONST_H
#define BENCHMARK_BENCHMARK_TRACK_XFB_CONST_H

#include <memory>
#include <utility>

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>

#include "benchmark_track_xfb.h"
#include "glsl.h"

namespace benchmark {
	class benchmark_track_xfb_const final : public benchmark_track_xfb {

	public:
		static bool checkSupport() noexcept;

		std::string getName() const noexcept final { return "benchmark_track_xfb_const"; };

		benchmark_track_xfb_const();

		std::pair<double, doRMSE_return_t> drawFrame() final;
	};
}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_TRACK_XFB_CONST_H
