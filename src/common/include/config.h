#ifndef COMMON_CONFIG_H
#define COMMON_CONFIG_H

#include <cstddef>
#include <string>

namespace common {
	enum Mode {
		RANDOM,
		RANDOM_SOL_ONLY,
		TRACK
	};

	constexpr std::size_t runs = 10;

	constexpr std::size_t windowWidth = 1920;
	constexpr std::size_t windowHeight = 1080;
	constexpr bool headless = true;

	extern std::size_t randomSeed;

	//Changes to this value have to also be made in the shaders!
	constexpr std::size_t maxSamples = 200000;
	constexpr std::size_t pointsPerTrack = 100;
	constexpr float trackStep = 0.1f;
	constexpr std::size_t scale_s = 4;
	constexpr std::size_t scale_tp = 4;

	extern std::size_t samples;
	extern std::size_t tracks;
	extern std::string method;
	extern std::size_t deviceID;
	extern Mode mode;
	extern std::string resultsDir;

	void configRead(const std::string &name);

#ifdef NDEBUG
	constexpr bool isDebug = false;
#else
	constexpr bool isDebug = true;
#endif
}// namespace common

#endif//COMMON_CONFIG_H
