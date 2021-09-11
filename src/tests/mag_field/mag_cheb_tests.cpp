#include <doctest/doctest.h>
#include <mag_cheb.h>
#include <memory>

TEST_CASE("Mag Cheb should initialize") {
	auto mag = std::make_unique<mag_field::mag_cheb>();

	SUBCASE("and have buffers") {
		CHECK(mag->getDipSegmentsPtr() != nullptr);
		CHECK(mag->getSolSegmentsPtr() != nullptr);
		CHECK(mag->getDipParamsPtr() != nullptr);
		CHECK(mag->getSolParamsPtr() != nullptr);
	}

	SUBCASE("and return correct values of field") {
		std::vector<glm::vec3> points = {
			glm::vec3(0, 0, 0),
			glm::vec3(0, 0, 550),
			glm::vec3(0, 0, -550),
			glm::vec3(-200, 0, 0),
			glm::vec3(200, 0, 0),
			glm::vec3(0, -200, 0),
			glm::vec3(0, 200, 0)
		};

		std::vector<glm::vec3> results = {
				glm::vec3(-5.28181e-05, -1.13088e-05, -5.00668),
				glm::vec3(0.0141443, -0.0119232, -3.37059),
				glm::vec3(0.189526, 0.0202912, -3.47661),
				glm::vec3(0.00345934, -0.000412402, -5.04039),
				glm::vec3(-0.000526554, -0.000162056, -5.0358),
				glm::vec3(0.000870721, 0.000391551, -5.02891),
				glm::vec3(0.000891177, -0.00086105, -5.04639)
		};

		for (std::size_t i = 0; i < points.size(); ++i) {
			auto const pos = points[i];
			auto const b = mag->Field(pos);
			for(std::size_t j = 0; j < b.length(); ++j) {
				CHECK(b[j] == doctest::Approx(results[i][j]).epsilon(0.001));
			}
		}
	}
}
