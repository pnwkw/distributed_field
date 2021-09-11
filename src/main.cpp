#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <config.h>
#include <filesystem>
#include <fstream>
#include <logger.h>

#include <ctime>

#ifdef USE_GLFW_CONTEXT
#include <context_glfw.h>
#endif

#ifdef USE_EGL_CONTEXT
#include <context_egl.h>
#endif

#include <benchmark_window.h>

int main() {
	auto logger_graphics = spdlog::stdout_color_mt("graphics");
	auto logger_field = spdlog::stdout_color_mt("field");

	if constexpr (common::isDebug) {
		common::graphics_logger()->set_level(spdlog::level::debug);
		common::field_logger()->set_level(spdlog::level::debug);
		spdlog::set_level(spdlog::level::debug);
	}

	std::set_terminate([]() -> void {
		try {
			std::rethrow_exception(std::current_exception());
		} catch (const std::exception &ex) {
			spdlog::error("{}", ex.what());
		}

		std::abort();
	});

	common::configRead("config.json");

#ifdef USE_GLFW_CONTEXT
	common::context_glfw context;
#endif

#ifdef USE_EGL_CONTEXT
	common::context_egl context;
#endif
	const bool changeSeed = (common::randomSeed == -1);

	{
		if (common::resultsDir != ".") {
			std::filesystem::create_directory(common::resultsDir);
		}

		auto benchmark_window = benchmark::benchmark_window(context);

		std::ofstream outfile(fmt::format("{}/{}_{}_{}.csv", common::resultsDir, benchmark_window.getBenchmarkName(), common::samples, common::mode));

		if (outfile.good()) {

			outfile << "Time;RMSE;RMSEx;RMSEy;RMSEz" << std::endl;

			double total_duration = 0, total_RMSEt = 0, total_RMSEx = 0, total_RMSEy = 0, total_RMSEz = 0;

			for (std::size_t i = 0; i < common::runs; i++) {
				if (changeSeed) {
					common::randomSeed = time(nullptr);
				}

				benchmark_window.startFrame();

				auto const [duration, RMSE] = benchmark_window.drawFrame();
				auto const [RMSEt, RMSEx, RMSEy, RMSEz] = RMSE;
				benchmark_window.endFrame();

				total_duration += duration;
				total_RMSEt += RMSEt;
				total_RMSEx += RMSEx;
				total_RMSEy += RMSEy;
				total_RMSEz += RMSEz;
			}

			total_duration /= common::runs;
			total_RMSEt /= common::runs;
			total_RMSEx /= common::runs;
			total_RMSEy /= common::runs;
			total_RMSEz /= common::runs;

			outfile << total_duration << ";" << total_RMSEt << ";" << total_RMSEx << ";" << total_RMSEy << ";" << total_RMSEz << std::endl;
		}

		outfile.close();
	}

	return EXIT_SUCCESS;
}
