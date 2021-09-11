#include "ssbo.h"

#include <fstream>

#include <config.h>
#include <logger.h>

bool benchmark::ssbo::checkSupport() noexcept {
	common::graphics_logger()->info("SSBO shader requirements satisfied by OpenGL core profile");
	return true;
}

benchmark::ssbo::ssbo() {
	loadFromBinary("ssbo_data.bin");

	gl::glCreateBuffers(1, &ssbo_buffer);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_BUFFER, ssbo_buffer, -1, "SSBO Field Buffer");
	}

	gl::glNamedBufferData(ssbo_buffer, fieldData.size() * sizeof(decltype(fieldData)::value_type), fieldData.data(), gl::GL_STATIC_DRAW);
}

benchmark::ssbo::~ssbo() {
	gl::glDeleteBuffers(1, &ssbo_buffer);
}

void benchmark::ssbo::loadFromBinary(const std::string &filename) {
	std::ifstream file;

	file.open("data/" + filename, std::ifstream::binary | std::ifstream::ate);

	if (!file.good()) {
		throw std::runtime_error("Cannot load data file!");
	}

	std::size_t len = static_cast<std::size_t>(file.tellg());
	file.seekg(0, std::ifstream::beg);

	std::uint32_t scale_s, scale_tp;

	file.read(reinterpret_cast<char *>(&scale_s), sizeof(std::uint32_t));
	file.read(reinterpret_cast<char *>(&scale_tp), sizeof(std::uint32_t));

	if (scale_s != common::scale_s || scale_tp != common::scale_tp) {
		throw std::runtime_error("Loaded binary file was saved with different scale settings than the program");
	}

	len -= sizeof(std::uint32_t) * 2;

	fieldData.resize(len / sizeof(glm::vec4));

	file.read(reinterpret_cast<char *>(this->fieldData.data()), len);

	common::graphics_logger()->debug("Data file loaded, scale_s: {} scale_tp: {}", scale_s, scale_tp);

	file.close();
}

gl::GLuint benchmark::ssbo::worldToIndex(const glm::ivec3 &w) {
	auto r = glm::ivec3(w.z + mag_field::z_detector - mag_field::center.z, w.x + mag_field::x_detector - mag_field::center.x, w.y + mag_field::y_detector - mag_field::center.y);
	r = r / scales;

	return (r.p * dimensions_scaled.s * dimensions_scaled.t) + r.t * dimensions_scaled.s + r.s;
}
