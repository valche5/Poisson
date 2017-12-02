project (Final)

cmake_minimum_required (VERSION 3.0.2)

set(CMAKE_CXX_STANDARD 11)

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
