project (final)

cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(Eigen)

set(SRCS
        main.cpp
		EigenImage.cpp
		SeamlessCloning.cpp

        stb_image.h
		EigenImage.h
		SeamlessCloning.h
		stb_image_write.h)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING
			"Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
			FORCE)
endif(NOT CMAKE_BUILD_TYPE)

add_executable(final ${SRCS})
target_link_libraries (final)
