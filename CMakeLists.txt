cmake_minimum_required(VERSION 2.8)
project(DictzipQuery)

set(
	CMAKE_CXX_FLAGS
	"-std=c++0x -W -Wall -Wextra -Winline -pedantic"
)

include_directories(
	src/
)

add_library(
	DictzipQuery
	SHARED
		src/istream/stream.cc
		src/istream/buffer.cc
)

add_executable(
	example
		example.cc
)

target_link_libraries(
	example
		DictzipQuery
		z
)

install(
	TARGETS
		DictzipQuery
	LIBRARY DESTINATION
		lib
)
