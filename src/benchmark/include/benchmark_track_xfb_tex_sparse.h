#ifndef BENCHMARK_BENCHMARK_TRACK_XFB_TEX_SPARSE_H
#define BENCHMARK_BENCHMARK_TRACK_XFB_TEX_SPARSE_H

#include <memory>
#include <utility>

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>

#include "benchmark_track_xfb.h"
#include "sparse.h"

namespace benchmark {
	class benchmark_track_xfb_tex_sparse final : public benchmark_track_xfb {
	private:
		std::unique_ptr<sparse> textureHandler;

	public:
		static bool checkSupport() noexcept;

		std::string getName() const noexcept final { return "benchmark_track_xfb_tex_sparse"; };

		benchmark_track_xfb_tex_sparse();

		std::pair<double, doRMSE_return_t> drawFrame() final;
	};
}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_TRACK_XFB_TEX_SPARSE_H
