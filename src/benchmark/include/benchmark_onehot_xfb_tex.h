#ifndef BENCHMARK_BENCHMARK_ONEHOT_XFB_TEX_H
#define BENCHMARK_BENCHMARK_ONEHOT_XFB_TEX_H

#include <memory>
#include <utility>

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>

#include "benchmark_onehot_xfb.h"
#include "texture.h"

namespace benchmark {
	class benchmark_onehot_xfb_tex final : public benchmark_onehot_xfb {
	private:
		std::unique_ptr<texture> textureHandler;

	public:
		static bool checkSupport() noexcept;

		std::string getName() const noexcept final { return "benchmark_onehot_xfb_tex"; };

		benchmark_onehot_xfb_tex();

		std::pair<double, doRMSE_return_t> drawFrame() final;
	};
}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_ONEHOT_XFB_TEX_H
