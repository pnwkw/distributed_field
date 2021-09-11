#include "benchmark_track_xfb.h"

#include <array>
#include <random>
#include <span>

#include <config.h>
#include <glm/gtc/constants.hpp>
#include <logger.h>
#include <mag_cheb.h>
#include <measurements.h>

#include <nlohmann/json.hpp>

bool benchmark::benchmark_track_xfb::checkSupport() noexcept {
	gl::GLint max_geometry_output_vertices;
	gl::glGetIntegerv(gl::GL_MAX_GEOMETRY_OUTPUT_VERTICES, &max_geometry_output_vertices);
	common::graphics_logger()->debug("GL_MAX_GEOMETRY_OUTPUT_VERTICES {}", max_geometry_output_vertices);

	return max_geometry_output_vertices > (common::samples / common::tracks);
}

benchmark::benchmark_track_xfb::benchmark_track_xfb() : points(), points_result(common::samples) {
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

	gl::glVertexArrayVertexBuffer(vao, benchmark_track_xfb::VTX_BINDING, vertex_buffer, 0, sizeof(Point));
	gl::glVertexArrayAttribBinding(vao, Point::POS_ATTRIB, benchmark_track_xfb::VTX_BINDING);
}

benchmark::benchmark_track_xfb::~benchmark_track_xfb() {
	gl::glDeleteProgram(program_xfb);

	gl::glDeleteBuffers(1, &xfb_buffer);
	gl::glDeleteBuffers(1, &vertex_buffer);

	gl::glDeleteVertexArrays(1, &vao);
}

benchmark::benchmark_base::doRMSE_return_t benchmark::benchmark_track_xfb::doRMSE(const bool useCache) const {
	auto mag = std::make_unique<mag_field::mag_cheb>();

	double RMSE = 0.0, RMSEx = 0.0, RMSEy = 0.0, RMSEz = 0.0;

	const auto cartToCyl = [](const glm::ivec3 &pos) {
	  	const auto btr = glm::length(glm::vec2(pos.x, pos.y));
	  	const auto psiPLUSphi = glm::atan(static_cast<float>(pos.y), static_cast<float>(pos.x)) + pos.y;

		return glm::ivec3(btr, psiPLUSphi, pos.z);
	};

	for (std::size_t t = 0; t < common::tracks; ++t) {
		auto currentPos = points[t];
		mag->resetCache();
		for (std::size_t i = 0; i < common::pointsPerTrack; ++i) {
			if (common::mode == common::Mode::RANDOM_SOL_ONLY) {
				const auto cyl = cartToCyl(points_result[t * common::pointsPerTrack + i]);

				if (cyl.z < mag_field::SolZMin || cyl.z >= mag_field::SolZMax || cyl.x >= mag_field::SolR) {
					break;
				}
			}

			const auto diffx = currentPos.x - points_result[t * common::pointsPerTrack + i].x;
			const auto diffy = currentPos.y - points_result[t * common::pointsPerTrack + i].y;
			const auto diffz = currentPos.z - points_result[t * common::pointsPerTrack + i].z;

			RMSEx += diffx * diffx;
			RMSEy += diffy * diffy;
			RMSEz += diffz * diffz;

			const auto distance = glm::distance(currentPos, points_result[t * common::pointsPerTrack + i]);
			RMSE += (distance * distance) / currentPos.length();

			glm::vec3 b_vec = mag->Field(currentPos, useCache);

			currentPos = currentPos - b_vec * common::trackStep;
		}
	}

	RMSE = glm::sqrt(RMSE / common::samples);
	RMSEx = glm::sqrt(RMSEx / common::samples);
	RMSEy = glm::sqrt(RMSEy / common::samples);
	RMSEz = glm::sqrt(RMSEz / common::samples);

	return {RMSE, RMSEx, RMSEy, RMSEz};
}

void benchmark::benchmark_track_xfb::clearResults() {
	decltype(points_result)::value_type const zero_point(0);

	std::fill(points_result.begin(), points_result.end(), zero_point);
}
void benchmark::benchmark_track_xfb::generatePoints() {
	points.clear();

	std::mt19937 rng(common::randomSeed);

	if (common::mode == common::Mode::RANDOM) {
		std::uniform_int_distribution<int> generator_x(mag_field::detector_min.x, mag_field::detector_max.x);
		std::uniform_int_distribution<int> generator_y(mag_field::detector_min.y, mag_field::detector_max.y);
		std::uniform_int_distribution<int> generator_z(mag_field::detector_min.z, mag_field::detector_max.z);

		for (std::size_t i = 0; i < common::tracks; ++i) {
			points.emplace_back(generator_x(rng), generator_y(rng), generator_z(rng));
		}
	} else if (common::mode == common::Mode::RANDOM_SOL_ONLY) {
		std::uniform_int_distribution<int> generator_r(0, mag_field::SolR);
		std::uniform_real_distribution<double> generator_phi(0, 2 * glm::pi<double>());
		std::uniform_int_distribution<int> generator_z(mag_field::SolZMin, mag_field::SolZMax);

		for (std::size_t i = 0; i < common::tracks; ++i) {
			const auto R = generator_r(rng);
			const auto Phi = generator_phi(rng);

			points.emplace_back(R * glm::cos(Phi), R * glm::sin(Phi), generator_z(rng));
		}
	}

	//Replace random data with a couple of test points
	if constexpr (common::isDebug) {
		std::array<glm::vec3, 5> dbg_points{
				glm::vec3(0, 0, 0),
				glm::vec3(0, 0, 550),
				glm::vec3(0, 0, -550),
				glm::vec3(-200, 0, 0),
				glm::vec3(200, 0, 0)};
		auto max_dbg_points = std::min(dbg_points.size(), common::tracks);

		for (std::size_t t = 0; t < max_dbg_points; t++) {
			points[t] = dbg_points[t];
		}
	}

	gl::glNamedBufferData(vertex_buffer, std::span(points).size_bytes(), points.data(), gl::GL_STATIC_DRAW);
	gl::glNamedBufferData(xfb_buffer, common::samples * sizeof(decltype(points)::value_type), nullptr, gl::GL_STREAM_READ);
}
