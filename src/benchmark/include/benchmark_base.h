#ifndef DISPLAY_BENCHMARK_BENCHMARK_BASE_H
#define DISPLAY_BENCHMARK_BENCHMARK_BASE_H

#include <memory>
#include <string>
#include <utility>

namespace benchmark {
	class benchmark_base {
	public:
		using doRMSE_return_t = std::tuple<double, double, double, double>;

		static bool checkSupport() noexcept { return true; };

		virtual ~benchmark_base() = default;

		virtual std::string getName() const noexcept = 0;

		virtual std::pair<double, doRMSE_return_t> drawFrame() = 0;

		virtual doRMSE_return_t doRMSE(bool useCache = false) const = 0;
	};
}// namespace benchmark

#endif//DISPLAY_BENCHMARK_BENCHMARK_BASE_H
