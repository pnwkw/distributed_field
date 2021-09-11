#ifndef BENCHMARK_BENCHMARK_WINDOW_H
#define BENCHMARK_BENCHMARK_WINDOW_H

#include <memory>
#include <utility>

#include <benchmark_base.h>

#include <context_base.h>

namespace benchmark {
	class benchmark_window {
	private:
		common::context_base &context;
		std::unique_ptr<benchmark::benchmark_base> benchmarkImpl{};

	public:
		benchmark_window(common::context_base &context);
		~benchmark_window();
		std::string getBenchmarkName() { return benchmarkImpl->getName(); };
		bool shouldClose() const { return context.shouldClose(); };
		void startFrame();
		std::pair<double, benchmark::benchmark_base::doRMSE_return_t> drawFrame();
		void endFrame();
	};

}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_WINDOW_H