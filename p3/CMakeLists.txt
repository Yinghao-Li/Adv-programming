cmake_minimum_required(VERSION 3.8)
find_package(CUDA QUIET REQUIRED)
find_package(MPI REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGES "${CMAKE_CXX_FLAGES}" -std=c++11)
set(CMAKE_CXX_FLAGS -pthread)

include_directories(
	SYSTEM
	${MPI_INCLUDE_PATH}
	src
	)

set(SOURCE_FILES
		src/complex.cc
		src/complex.h
		src/input_image.cc
		src/input_image.h
		src/helper_cuda.h
		src/helper_string.h
	)

add_executable(p31 thr_fft2d.cc ${SOURCE_FILES})
add_executable(p32 MPI_fft2d.cc ${SOURCE_FILES})
cuda_add_executable(p33 CUDA_fft2d.cu ${SOURCE_FILES})
target_link_libraries(p32 ${MPI_CXX_LIBRARIES})
