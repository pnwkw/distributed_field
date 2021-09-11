add_library(benchmark STATIC)
add_library(benchmark::benchmark ALIAS benchmark)

target_include_directories(benchmark PUBLIC include)

target_sources(benchmark PRIVATE debug_render_texture.cpp)
target_link_libraries(benchmark benchmark::program::debug)

target_sources(benchmark PRIVATE benchmark_window.cpp benchmark_onehot_xfb.cpp benchmark_track_xfb.cpp)

target_sources(benchmark PRIVATE texture.cpp sparse.cpp ssbo.cpp glsl.cpp)

target_sources(benchmark PRIVATE benchmark_onehot_xfb_const.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_vert_const)

target_sources(benchmark PRIVATE benchmark_onehot_xfb_tex.cpp)
target_sources(benchmark PRIVATE benchmark_onehot_xfb_tex_sparse.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_vert_texture)

target_sources(benchmark PRIVATE benchmark_onehot_xfb_ssbo.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_vert_ssbo)

target_sources(benchmark PRIVATE benchmark_onehot_xfb_glsl.cpp)
target_sources(benchmark PRIVATE benchmark_onehot_xfb_glsl_cache.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_vert_glsl)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_vert_glsl_cache)

target_sources(benchmark PRIVATE benchmark_track_xfb_const.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_geom_const)

target_sources(benchmark PRIVATE benchmark_track_xfb_tex.cpp)
target_sources(benchmark PRIVATE benchmark_track_xfb_tex_sparse.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_geom_texture)

target_sources(benchmark PRIVATE benchmark_track_xfb_glsl.cpp)
target_sources(benchmark PRIVATE benchmark_track_xfb_glsl_cache.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_geom_glsl)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_geom_glsl_cache)

target_sources(benchmark PRIVATE benchmark_track_xfb_ssbo.cpp)
target_link_libraries(benchmark benchmark::program::benchmark_xfb_geom_ssbo)

target_link_libraries(benchmark common::common glbinding::glbinding glbinding::glbinding-aux glm::glm mag_field::mag_field spdlog::spdlog)
