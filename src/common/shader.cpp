#include "shader.h"

#include <config.h>
#include <fstream>
#include <logger.h>

std::vector<std::uint8_t> common::loadShaderSpirv(const std::string &shaderFilename) {
	std::fstream file;

	std::ifstream input("shaders/" + shaderFilename + ".spv", std::ios::binary);

	if (input.good()) {
		std::vector<std::uint8_t> buffer(std::istreambuf_iterator<char>(input), {});

		return buffer;
	}

	throw std::runtime_error("Cannot load shader!");
};

std::vector<gl::GLchar> common::loadShaderGlsl(const std::string &shaderFilename) {
	std::fstream file;

	std::ifstream input("shaders/" + shaderFilename + ".glsl", std::ios::binary);

	if (input.good()) {
		std::vector<gl::GLchar> buffer(std::istreambuf_iterator<char>(input), {});

		return buffer;
	}

	throw std::runtime_error("Cannot load shader!");
}

gl::GLuint common::loadCompileShader(gl::GLenum shaderType, const std::string &shaderFilename, const Specialization *spec) {
#ifdef USE_GLSL_SHADERS
	return loadCompileShaderGlsl(shaderType, shaderFilename, spec);
#endif

#ifdef USE_SPIRV_SHADERS
	return loadCompileShaderSpirv(shaderType, shaderFilename, spec);
#endif
}

gl::GLuint common::loadCompileShaderSpirv(gl::GLenum shaderType, const std::string &shaderFilename, const Specialization *spec) {
	const auto buffer = common::loadShaderSpirv(shaderFilename);

	if (buffer.empty()) {
		common::graphics_logger()->error("Can't read shader file {}", shaderFilename);
		throw std::runtime_error("");
	}

	gl::GLuint shader = gl::glCreateShader(shaderType);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_SHADER, shader, -1, shaderFilename.c_str());
	}

	gl::glShaderBinary(1, &shader, gl::GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, buffer.data(), buffer.size());

	if (spec) {
		gl::glSpecializeShader(shader, "main", spec->indexes.size(), spec->indexes.data(), spec->values.data());
	} else {
		gl::glSpecializeShader(shader, "main", 0, nullptr, nullptr);
	}

	gl::GLint error_status;

	gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &error_status);
	if (!error_status) {
		common::graphics_logger()->error("Error compiling shader {}", shaderFilename);

		gl::GLint maxLength = 0;
		gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<gl::GLchar> infoLog(maxLength);
		gl::glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

		common::graphics_logger()->error(infoLog.data());

		gl::glDeleteShader(shader);

		throw std::runtime_error("");
	}

	return shader;
}

gl::GLuint common::loadCompileShaderGlsl(gl::GLenum shaderType, const std::string &shaderFilename, const Specialization *spec) {
	auto buffer = common::loadShaderGlsl(shaderFilename);

	if (buffer.empty()) {
		common::graphics_logger()->error("Can't read shader file {}", shaderFilename);
		throw std::runtime_error("");
	}

	gl::GLuint shader = gl::glCreateShader(shaderType);
	if constexpr (common::isDebug) {
		gl::glObjectLabel(gl::GL_SHADER, shader, -1, shaderFilename.c_str());
	}

	const auto ptr = buffer.data();
	const auto len = static_cast<gl::GLint>(buffer.size());
	gl::glShaderSource(shader, 1, &ptr, &len);

	gl::glCompileShader(shader);

	gl::GLint error_status;

	gl::glGetShaderiv(shader, gl::GL_COMPILE_STATUS, &error_status);
	if (!error_status) {
		common::graphics_logger()->error("Error compiling shader {}", shaderFilename);

		gl::GLint maxLength = 0;
		gl::glGetShaderiv(shader, gl::GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<gl::GLchar> infoLog(maxLength);
		gl::glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog.data());

		common::graphics_logger()->error(infoLog.data());

		gl::glDeleteShader(shader);

		throw std::runtime_error("Error compiling shader");
	}

	return shader;
}

gl::GLuint common::createProgram(const std::initializer_list<gl::GLuint> &shaderVector, const char *feedback) {
	gl::GLuint program = gl::glCreateProgram();

	for (const auto shaderObj : shaderVector) {
		gl::glAttachShader(program, shaderObj);
	}

	if (feedback) {
		const char *ptr = feedback;
		gl::glTransformFeedbackVaryings(program, 1, &ptr, gl::GL_INTERLEAVED_ATTRIBS);
	}

	gl::glLinkProgram(program);

	gl::GLint error_status;

	gl::glGetProgramiv(program, gl::GL_LINK_STATUS, &error_status);

	if (!error_status) {
		common::graphics_logger()->error("Error linking program");

		gl::GLint maxLength = 0;
		gl::glGetProgramiv(program, gl::GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<gl::GLchar> infoLog(maxLength);
		gl::glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.data());

		common::graphics_logger()->error(infoLog.data());

		gl::glDeleteProgram(program);

		throw std::runtime_error("Error linking program");
	}

	return program;
}
