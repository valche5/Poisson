project (Final)

cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(Eigen)

find_package(OpenMP)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(SRCS
        main.cpp
		imdisplay.cpp

		imdisplay.h
		Image.h
        stb_image.h
        CImg.h)

find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

add_executable(final ${SRCS})
target_link_libraries (final ${CMAKE_THREAD_LIBS_INIT} ${X11_LIBRARIES})
