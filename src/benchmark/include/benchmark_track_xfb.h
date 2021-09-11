#ifndef BENCHMARK_BENCHMARK_TRACK_XFB_H
#define BENCHMARK_BENCHMARK_TRACK_XFB_H

#include <utility>
#include <vector>

#include <benchmark_base.h>
#include <glbinding/gl46core/gl.h>
#include <glm/glm.hpp>

namespace benchmark {
	class benchmark_track_xfb : public benchmark_base {
	protected:
		gl::GLuint vao{};
		gl::GLuint vertex_buffer{};
		constexpr static gl::GLuint VTX_BINDING = 0;

		gl::GLuint xfb_buffer{};

		gl::GLuint program_xfb{};

		struct Point {
			glm::vec3 pos;
			constexpr static gl::GLuint POS_ATTRIB = 0;
		};

		std::vector<decltype(Point::pos)> points;
		std::vector<decltype(Point::pos)> points_result;

	public:
		static bool checkSupport() noexcept;

		benchmark_track_xfb();
		~benchmark_track_xfb() override;

		void generatePoints();
		void clearResults();

		doRMSE_return_t doRMSE(bool useCache = false) const final;
	};
}// namespace benchmark

#endif//BENCHMARK_BENCHMARK_TRACK_XFB_H
