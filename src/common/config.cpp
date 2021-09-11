#include "config.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace common {
	std::size_t samples;
	std::size_t tracks;
	std::string method;
	std::size_t deviceID;
	Mode mode;
	std::string resultsDir;
	std::size_t randomSeed;

	void configRead(const std::string &name) {
		std::ifstream i(name);
		nlohmann::json j;
		i >> j;

		samples = j["samples"];

		if (samples > maxSamples) {
			throw std::runtime_error("Samples exceed maxSamples!");
		}
		tracks = samples / pointsPerTrack;

		method = j["method"];

		if (j.find("deviceID") != j.end()) {
			deviceID = j["deviceID"];
		} else {
			deviceID = 0;
		}

		if (j.find("randomSeed") != j.end()) {
			randomSeed = j["randomSeed"];
		} else {
			randomSeed = -1;
		}

		if (j.find("mode") != j.end()) {
			switch (j["mode"].get<int>()) {
				case 0:
					mode = Mode::RANDOM;
					break;
				case 1:
					mode = Mode::RANDOM_SOL_ONLY;
					break;
				case 2:
					mode = Mode::TRACK;
					break;
				default:
					mode = Mode::RANDOM;
					break;
			}
		} else {
			mode = Mode::RANDOM;
		}

		if (j.find("resultsDir") != j.end()) {
			resultsDir = j["resultsDir"];
		} else {
			resultsDir = ".";
		}
	}
}// namespace common
