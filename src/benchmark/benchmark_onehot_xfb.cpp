#include "benchmark_onehot_xfb.h"

#include <random>
#include <span>

#include <config.h>
#include <logger.h>
#include <measurements.h>

#include <mag_cheb.h>

#include <glm/gtc/constants.hpp>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool benchmark::benchmark_onehot_xfb::checkSupport() noexcept {
	common::graphics_logger()->info("Transform Feedback from Vertex Shader requirements satisfied by OpenGL core profile");
	return true;
}

benchmark::benchmark_onehot_xfb::benchmark_onehot_xfb() : points(), points_result(common::samples) {
	gl::glCreateVertexArrays(1, &vao);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_VERTEX_ARRAY, vao, -1, "Vertex Array");
	}

	gl::glCreateBuffers(1, &vertex_buffer);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_BUFFER, vertex_buffer, -1, "Vertex buffer");
	}

	gl::glCreateBuffers(1, &xfb_buffer);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_BUFFER, xfb_buffer, -1, "Transform Feeback buffer");
	}

	gl::glEnableVertexArrayAttrib(vao, Point::POS_ATTRIB);
	gl::glVertexArrayAttribFormat(vao, Point::POS_ATTRIB, decltype(Point::pos)::length(), gl::GL_FLOAT, gl::GL_FALSE, offsetof(Point, pos));

	gl::glVertexArrayVertexBuffer(vao, benchmark_onehot_xfb::VTX_BINDING, vertex_buffer, 0, sizeof(Point));
	gl::glVertexArrayAttribBinding(vao, Point::POS_ATTRIB, benchmark_onehot_xfb::VTX_BINDING);
}

benchmark::benchmark_onehot_xfb::~benchmark_onehot_xfb() {
	gl::glDeleteProgram(program_xfb);

	gl::glDeleteBuffers(1, &xfb_buffer);
	gl::glDeleteBuffers(1, &vertex_buffer);

	gl::glDeleteVertexArrays(1, &vao);
}

benchmark::benchmark_base::doRMSE_return_t benchmark::benchmark_onehot_xfb::doRMSE(const bool useCache) const {
	auto mag = std::make_unique<mag_field::mag_cheb>();

	double RMSE = 0.0, RMSEx = 0.0, RMSEy = 0.0, RMSEz = 0.0;

	for (std::size_t i = 0; i < common::samples; ++i) {
		const glm::dvec3 v1 = mag->Field(points[i], useCache);
		const glm::dvec3 v2 = points_result[i];

		const auto diff = v1 - v2;

		RMSEx += diff.x * diff.x;
		RMSEy += diff.y * diff.y;
		RMSEz += diff.z * diff.z;

		const auto distance = glm::distance(v1, v2);
		RMSE += (distance * distance) / (decltype(v1)::length());
	}

	RMSE = glm::sqrt(RMSE / common::samples);
	RMSEx = glm::sqrt(RMSEx / common::samples);
	RMSEy = glm::sqrt(RMSEy / common::samples);
	RMSEz = glm::sqrt(RMSEz / common::samples);

	return {RMSE, RMSEx, RMSEy, RMSEz};
}

void benchmark::benchmark_onehot_xfb::generatePoints() {
	points.clear();

	std::mt19937 rng(common::randomSeed);

	if (common::mode == common::Mode::RANDOM) {
		std::uniform_int_distribution<int> generator_x(mag_field::detector_min.x, mag_field::detector_max.x);
		std::uniform_int_distribution<int> generator_y(mag_field::detector_min.y, mag_field::detector_max.y);
		std::uniform_int_distribution<int> generator_z(mag_field::detector_min.z, mag_field::detector_max.z);

		for (std::size_t i = 0; i < common::samples; ++i) {
			points.emplace_back(generator_x(rng), generator_y(rng), generator_z(rng));
		}
	} else if (common::mode == common::Mode::RANDOM_SOL_ONLY) {
		std::uniform_int_distribution<int> generator_r(0, mag_field::SolR);
		std::uniform_real_distribution<double> generator_phi(0, 2 * glm::pi<double>());
		std::uniform_int_distribution<int> generator_z(mag_field::SolZMin, mag_field::SolZMax);

		for (std::size_t i = 0; i < common::samples; ++i) {
			const auto R = generator_r(rng);
			const auto Phi = generator_phi(rng);

			points.emplace_back(R * glm::cos(Phi), R * glm::sin(Phi), generator_z(rng));
		}
	} else if (common::mode == common::Mode::TRACK) {
		std::ifstream in("data/events.json");
		json jfile;
		in >> jfile;

		int totalPoints = 0;

		for (std::size_t idx = 0; idx < jfile.size(); idx++) {
			auto event = jfile[idx];

			for (std::size_t trackidx = 0; trackidx < event["tracks"].size(); trackidx++) {
				auto track = event["tracks"][trackidx];
				for (std::size_t pointidx = 0; pointidx < track["trajectory"].size(); pointidx++) {
					auto point = track["trajectory"][pointidx];

					points.emplace_back(point[0], point[1], point[2]);
					totalPoints++;

					if (points.size() == common::samples) {
						goto done;
					}
				}
			}
		}
		done:
		spdlog::info("Total points read {}", totalPoints);
	}

	//Replace random data with a couple of test points
	if constexpr (common::isDebug) {
		points[0] = glm::vec3(0, 0, 0);
		points[1] = glm::vec3(0, 0, 550);
		points[2] = glm::vec3(0, 0, -550);
		points[3] = glm::vec3(-200, 0, 0);
		points[4] = glm::vec3(200, 0, 0);
		points[5] = glm::vec3(0, -200, 0);
		points[6] = glm::vec3(0, 200, 0);
	}

	gl::glNamedBufferData(vertex_buffer, std::span(points).size_bytes(), points.data(), gl::GL_STATIC_DRAW);
	gl::glNamedBufferData(xfb_buffer, common::samples * sizeof(decltype(points)::value_type), nullptr, gl::GL_STREAM_READ);
}

void benchmark::benchmark_onehot_xfb::clearResults() {
	decltype(points_result)::value_type const zero_point(0);

	std::fill(points_result.begin(), points_result.end(), zero_point);
}
