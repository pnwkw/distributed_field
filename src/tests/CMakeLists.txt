add_library(tests OBJECT)
target_link_libraries(tests common::common benchmark::benchmark spdlog::spdlog doctest::doctest)

target_sources(tests PRIVATE mag_field/mag_cheb_tests.cpp)

if (DEFINED USE_GLFW_CONTEXT)
    target_sources(tests PRIVATE common/context_glfw_tests.cpp)
    target_link_libraries(tests common::context_glfw)
endif (DEFINED USE_GLFW_CONTEXT)

if (DEFINED USE_EGL_CONTEXT)
    target_sources(tests common/context_egl_tests.cpp)
    target_link_libraries(tests common::context_egl)
endif (DEFINED USE_EGL_CONTEXT)