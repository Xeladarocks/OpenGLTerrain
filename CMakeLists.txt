# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

project ("OpenGLCMake")

# ----= Perlin =----- #
include_directories("extern/perlin/")
# ----=        =----- #
# ----= glm =----- #
include_directories("extern/glm/")
# ----=     =----- #

add_subdirectory ("extern")
add_subdirectory ("src")
