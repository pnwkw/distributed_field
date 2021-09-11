#ifndef BENCHMARK_SPARSE_H
#define BENCHMARK_SPARSE_H

#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>
#include <glm_helper.h>

#include <config.h>
#include <context_base.h>
#include <mag_cheb.h>
#include <measurements.h>

namespace benchmark {
	class sparse {
	private:
		gl::GLuint texture3d{};

		common::unordered_ivec3_set pages{};
		glm::ivec3 sparse_page_sizes{16, 16, 16};

		constexpr static glm::ivec3 scales{common::scale_s, common::scale_tp, common::scale_tp};
		glm::ivec3 texture_dimensions_scaled{};

		std::unique_ptr<mag_field::mag_cheb> mag;

	public:
		sparse();
		~sparse();

		gl::GLuint getTexture() const { return texture3d; };

		static bool checkSupport() noexcept;
		static glm::ivec3 worldToTex(glm::ivec3 const &w);
		static glm::ivec3 texToWorld(glm::ivec3 const &t);

		bool dbgIsInside(glm::ivec3 const &w);
		void dbgProjectionZX();
		void dbgProjectionZY();
		void dbgProjectionXY();
	};
}// namespace benchmark

#endif//BENCHMARK_SPARSE_H
