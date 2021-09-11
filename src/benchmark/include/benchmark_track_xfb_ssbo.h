#ifndef BENCHMARK_BENCHMARK_TRACK_XFB_SSBO_H
#define BENCHMARK_BENCHMARK_TRACK_XFB_SSBO_H

#include <memory>
#include <utility>

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>

#include "benchmark_track_xfb.h"
#include "ssbo.h"

namespace benchmark {
	class benchmark_track_xfb_ssbo final : public benchmark_track_xfb {
	private:
		std::unique_ptr<ssbo> ssboHandler;

	public:
		static bool checkSupport() noexcept;

		std::string getName() const noexcept final { return "benchmark_track_xfb_ssbo"; };

		benchmark_track_xfb_ssbo();

		std::pair<double, doRMSE_return_t> drawFrame() final;
	};
}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_TRACK_XFB_SSBO_H
