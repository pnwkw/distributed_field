# Distributed Simulation And Visualization of The ALICE Detector Magnetic Field

This repository contains supplemental code for performance evaluation of different methods of storage and access of magnetic field data on the GPU, developed for
our paper *Distributed Simulation And Visualization of The ALICE Detector Magnetic Field*.


## Installation

To build the software you need to install a C++ compiler and CMake, which can be done the easiest way by using the system package manager.

Then, clone the repository to a local directory on your system by issuing the following command:
```
git clone --recurse-submodules https://github.com/pnwkw/distributed_field
```
It will fetch the contents of the repository as well as code of all used libraries.

If you want to run the program on a Linux machine running in text-mode (i.e. without a graphical interface such as *Gnome* or *KDE*) you need to change the OpenGL provider used
by the program from GLFW3 to EGL.

This can be done by commenting out the `set(USE_GLFW_CONTEXT true)` and uncommenting `set(USE_EGL_CONTEXT true)` in the `CMakeLists.txt` (lines 8 and 9):
```
# EGL can be used to execute on systems with no graphics (text mode), otherwise GLFW
#set(USE_GLFW_CONTEXT true)
set(USE_EGL_CONTEXT true)
```

If using Windows or Linux with a graphical interface installed, then the default GLFW3 can be used and no additional changes are needed.

The next step is to create a build directory with:
```
cmake -B build
```
CMake will generate build scripts for you. To compile the program, type:
```
cmake -B build -S .
```

## Benchmarks

First, move to the directory where the executable resides:
```
cd build
```
### :warning: Missing data file :warning:
Some of the benchmarks require to run a large data file which was too big for inclusion in the repository itself.
Please download [ssbo_data.bin](https://github.com/pnwkw/distributed_field/releases/download/ssbo/ssbo_data.bin) from the Releases section of this repository and put it
in the `build/data` directory where `dip_params.bin`, `dip_segments.bin`, `events.json`, `sol_params.bin` and `sol_segments.bin` already resides.

### Configuration
In the build directory an example `config.json` was automatically created. Here, the benchmark application can be configured to perform a specific experimental scenario.

Options:

* `samples`: The number of evaluated points in 3D space. Any number between 100 and 200000 can be inserted.
* `mode`: If set to 0, then fully random points in 3D space will be generated.
   If set to 1, then random points will be generated only in the space occupied by ALICE solenoid magnet.
   If set to 2, then points will be taken from the `events.json` file and not randomly generated.
* `method`: Controls which type of benchmark will be executed (*onehot* or *track*)
   and which method of magnetic field data access will be used (*constant*, *glsl*, *glsl with cache*, *shader storage buffer*, *texture* or *sparse texture*). Please refer to
   the original article for more information.
   
   Possible values:
  * benchmark_onehot_xfb_const
  * benchmark_onehot_xfb_glsl
  * benchmark_onehot_xfb_glsl_cache
  * benchmark_onehot_xfb_ssbo
  * benchmark_onehot_xfb_tex
  * benchmark_onehot_xfb_tex_sparse
  * benchmark_track_xfb_const
  * benchmark_track_xfb_glsl
  * benchmark_track_xfb_glsl_cache
  * benchmark_track_xfb_ssbo
  * benchmark_track_xfb_tex
  * benchmark_track_xfb_tex_sparse
### Running
Execute the benchmark application with the following command:
```
./display_benchmark
```
The application will run and record the measurements (time spent rendering and model accuracy) to a CSV file.
