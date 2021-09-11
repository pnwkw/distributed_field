#ifndef BENCHMARK_SSBO_H
#define BENCHMARK_SSBO_H

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>
#include <glm_helper.h>

#include <config.h>
#include <context_base.h>
#include <mag_cheb.h>
#include <measurements.h>

namespace benchmark {
	class ssbo {
	private:
		constexpr static glm::ivec3 scales{common::scale_s, common::scale_tp, common::scale_tp};
		constexpr static glm::ivec3 dimensions_scaled = glm::ivec3(mag_field::detector_dimensions.z, mag_field::detector_dimensions.x, mag_field::detector_dimensions.y) / scales;

		gl::GLuint ssbo_buffer{};

		std::vector<glm::vec4> fieldData;

	public:
		ssbo();
		~ssbo();

		gl::GLuint getSsboBufferName() { return ssbo_buffer; };

		void loadFromBinary(const std::string &filename);
		static bool checkSupport() noexcept;
		static gl::GLuint worldToIndex(glm::ivec3 const &w);
	};
}// namespace benchmark

#endif//BENCHMARK_SSBO_H
