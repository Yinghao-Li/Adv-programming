include(CMakeToolsHelpers OPTIONAL)

cmake_minimum_required(VERSION 3.8)
project(se2)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH} )

add_executable(heat1D src/heat1D.cc)
target_link_libraries(heat1D PRIVATE ${MPI_CXX_LIBRARIES})
