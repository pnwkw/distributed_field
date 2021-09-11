#include "benchmark_window.h"

#include <glbinding-aux/ContextInfo.h>
#include <glbinding-aux/Meta.h>

#include <config.h>
#include <logger.h>

#include <benchmark_onehot_xfb_const.h>
#include <benchmark_onehot_xfb_glsl.h>
#include <benchmark_onehot_xfb_glsl_cache.h>
#include <benchmark_onehot_xfb_ssbo.h>
#include <benchmark_onehot_xfb_tex.h>
#include <benchmark_onehot_xfb_tex_sparse.h>

#include <benchmark_track_xfb_const.h>
#include <benchmark_track_xfb_glsl.h>
#include <benchmark_track_xfb_glsl_cache.h>
#include <benchmark_track_xfb_ssbo.h>
#include <benchmark_track_xfb_tex.h>
#include <benchmark_track_xfb_tex_sparse.h>

void GL_APIENTRY GLerror_callback(gl::GLenum source, gl::GLenum type, gl::GLuint id, gl::GLenum severity, gl::GLsizei length, const gl::GLchar *message, const void *userParam) {
	if (severity != gl::GL_DEBUG_SEVERITY_NOTIFICATION) {
		switch (type) {
			case gl::GLenum::GL_DEBUG_TYPE_ERROR:
				common::graphics_logger()->error("[{}] {}", glbinding::aux::Meta::getString(severity), message);
				break;
			default:
				common::graphics_logger()->info("[{}] {}", glbinding::aux::Meta::getString(severity), message);
				break;
		}
	}
}

benchmark::benchmark_window::benchmark_window(common::context_base &context) : context(context) {
	context.create();

	glbinding::initialize(context.getProcAddressPtr(), true);

	if constexpr (common::isDebug) {
		if (glbinding::aux::ContextInfo::supported({gl::GLextension::GL_KHR_debug})) {
			gl::ContextFlagMask ctxFlag;
			gl::glGetIntegerv(gl::GL_CONTEXT_FLAGS, reinterpret_cast<gl::GLint *>(&ctxFlag));
			if ((ctxFlag & gl::GL_CONTEXT_FLAG_DEBUG_BIT) == gl::GL_CONTEXT_FLAG_DEBUG_BIT) {
				gl::glDebugMessageCallback(GLerror_callback, nullptr);
				gl::glEnable(gl::GL_DEBUG_OUTPUT);
				gl::glEnable(gl::GL_DEBUG_OUTPUT_SYNCHRONOUS);
			}
		} else {
			common::graphics_logger()->warn("{} not supported. OpenGL logging disabled.", glbinding::aux::Meta::getString(gl::GLextension::GL_KHR_debug));
		}
	}

	common::graphics_logger()->info("Selected GPU: {}", common::deviceID);
	common::graphics_logger()->info("Current GPU: {}", glbinding::aux::ContextInfo::renderer());
	common::graphics_logger()->info("Vendor: {}", glbinding::aux::ContextInfo::vendor());
	const auto ver = glbinding::aux::ContextInfo::version();
	common::graphics_logger()->info("OpenGL Version: {}.{}", ver.majorVersion(), ver.minorVersion());

	if constexpr (common::isDebug) {
		common::graphics_logger()->debug("OpenGL Extensions:");
		const auto extensions = glbinding::aux::ContextInfo::extensions();
		for (const auto &ext : extensions) {
			common::graphics_logger()->debug("\t{}", glbinding::aux::Meta::getString(ext));
		}
	}

	common::graphics_logger()->info("selected method {} in mode: {}", common::method, common::mode);

	const std::string unsupportedMessage = "Unsupported required features!";

	if (common::method == "benchmark_onehot_xfb_const") {
		if (!benchmark_onehot_xfb_const::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_onehot_xfb_const>();
	} else if (common::method == "benchmark_onehot_xfb_glsl") {
		if (!benchmark_onehot_xfb_glsl::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_onehot_xfb_glsl>();
	} else if (common::method == "benchmark_onehot_xfb_glsl_cache") {
		if (!benchmark_onehot_xfb_glsl_cache::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_onehot_xfb_glsl_cache>();
	} else if (common::method == "benchmark_onehot_xfb_ssbo") {
		if (!benchmark_onehot_xfb_ssbo::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_onehot_xfb_ssbo>();
	} else if (common::method == "benchmark_onehot_xfb_tex") {
		if (!benchmark_onehot_xfb_tex::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_onehot_xfb_tex>();
	} else if (common::method == "benchmark_onehot_xfb_tex_sparse") {
		if (!benchmark_onehot_xfb_tex_sparse::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_onehot_xfb_tex_sparse>();
	} else if (common::method == "benchmark_track_xfb_const") {
		if (!benchmark_track_xfb_const::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_track_xfb_const>();
	} else if (common::method == "benchmark_track_xfb_glsl") {
		if (!benchmark_track_xfb_glsl::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_track_xfb_glsl>();
	} else if (common::method == "benchmark_track_xfb_glsl_cache") {
		if (!benchmark_track_xfb_glsl_cache::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_track_xfb_glsl_cache>();
	} else if (common::method == "benchmark_track_xfb_ssbo") {
		if (!benchmark_track_xfb_ssbo::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_track_xfb_ssbo>();
	} else if (common::method == "benchmark_track_xfb_tex") {
		if (!benchmark_track_xfb_tex::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_track_xfb_tex>();
	} else if (common::method == "benchmark_track_xfb_tex_sparse") {
		if (!benchmark_track_xfb_tex_sparse::checkSupport()) {
			throw std::runtime_error(unsupportedMessage);
		}

		benchmarkImpl = std::make_unique<benchmark_track_xfb_tex_sparse>();
	} else {
		throw std::runtime_error("Config benchmark method not found!");
	}

	spdlog::info("Running {}", benchmarkImpl->getName());
}

benchmark::benchmark_window::~benchmark_window() {
	gl::glFinish();
}

void benchmark::benchmark_window::startFrame() {
	context.pollEvents();
}

std::pair<double, benchmark::benchmark_base::doRMSE_return_t> benchmark::benchmark_window::drawFrame() {
	return benchmarkImpl->drawFrame();
}

void benchmark::benchmark_window::endFrame() {
	if constexpr (!common::headless) {
		context.swapBuffers();
	}
}
