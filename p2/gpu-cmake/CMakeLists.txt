cmake_minimum_required(VERSION 3.9)
project(p2)

find_package(CUDA REQUIRED)

set(CMAKE_CXX_FLAGES "${CMAKE_CXX_FLAGES}" -std=c++11)

set(CUDA_NVCC_FLAGES ${CUDA_NVCC_FLAGES};
	-03 -gencode arch=compute_22, code=sm_22
	)

include_directories(
	src
)

set(SOURCE_FILES
	src/heat.h
	src/helper_cuda.h
	src/helper_string.h
	)

cuda_add_executable(
	heat2D3D
	heat.cu
	src/heat.h
	src/helper_cuda.h
	src/helper_string.h
)

	


