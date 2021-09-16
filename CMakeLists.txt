cmake_minimum_required(VERSION 3.17)

project(display_benchmark VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# EGL can be used to execute on systems with no graphics (text mode), otherwise GLFW
set(USE_GLFW_CONTEXT true)
#set(USE_EGL_CONTEXT true)

# Fallback implementation of the algorithm available if can't use AliROOT (i.e. on Windows)
#set(USE_ROOT true)

# SPIRV shaders do not work with cache version of shaders due to gl_VertexID incompatibility
#set(USE_SPIRV_SHADERS true)
set(USE_GLSL_SHADERS true)

# doctest
add_subdirectory(external/doctest)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DDOCTEST_CONFIG_DISABLE")

# GLFW
if(DEFINED USE_GLFW_CONTEXT)
    set(GLFW_BUILD_DOCS        OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS       OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
    set(GLFW_USE_HYBRID_HPG    ON  CACHE BOOL "" FORCE)
    add_subdirectory(external/glfw)
endif(DEFINED USE_GLFW_CONTEXT)

# EGL
if(DEFINED USE_EGL_CONTEXT)
    add_subdirectory(external/egl)
endif(DEFINED USE_EGL_CONTEXT)

# GLM
add_subdirectory(external/glm)

if(DEFINED USE_GLSL_SHADERS)
    include(cmake/BuildGLSL.cmake)
endif(DEFINED USE_GLSL_SHADERS)

if(DEFINED USE_SPIRV_SHADERS)
    include(cmake/BuildSPIRV.cmake)
    # We need Vulkan compiler to compile SPIRV shaders
    find_package(Vulkan REQUIRED)
endif(DEFINED USE_SPIRV_SHADERS)

# glbinding
set(BUILD_SHARED_LIBS      OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES  OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS     OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glbinding)
# spdlog
add_subdirectory(external/spdlog)
# json
add_subdirectory(external/json)

# code
add_subdirectory(src)
# shaders
add_subdirectory(shaders)

# data files
file(COPY data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# ROOT stuff
if(DEFINED USE_ROOT)
#    set(ROOTSYS /home/user/alice/sw/ubuntu1604_x86-64/ROOT/latest)
    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
    list(APPEND CMAKE_PREFIX_PATH ${ROOTSYS})

    find_package(ROOT 6.20 CONFIG REQUIRED)

    include("${ROOT_DIR}/RootMacros.cmake")

    set(HDRS AliMagF.h AliMagFast.h AliMagWrapCheb.h AliCheb3D.h AliCheb3DCalc.h TVirtualMagField.h)
    set(SRCS AliMagF.cxx AliMagFast.cxx AliMagWrapCheb.cxx AliCheb3D.cxx AliCheb3DCalc.cxx)

    root_generate_dictionary("G__alimag" "${HDRS}" MODULE "alimag" LINKDEF "LinkDef.h" OPTIONS -noIncludePaths)

    add_library(alice_field STATIC)
    add_library(mag_field::alice_field ALIAS alice_field)

    target_sources(alice_field PRIVATE ${HDRS} ${SRCS} LinkDef.h G__alimag.cxx)

    target_include_directories(alice_field PUBLIC ${CMAKE_SOURCE_DIR})
    target_link_libraries(alice_field ROOT::Core ROOT::Hist)
endif(DEFINED USE_ROOT)
