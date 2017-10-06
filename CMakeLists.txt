cmake_minimum_required(VERSION 2.8)
project(DictzipQuery)

set(
	CMAKE_CXX_FLAGS
	"-std=c++17 -W -Wall -Wextra -Winline -pedantic"
)

include_directories(
	src/
)

add_library(
	DictzipQuery
	SHARED
		src/util/base64.cc
		src/istream/stream.cc
		src/istream/buffer.cc
)

target_link_libraries(
	DictzipQuery
		z
)

add_executable(
	example
		example.cc
)

target_link_libraries(
	example
		DictzipQuery
)

install(
	TARGETS
		DictzipQuery
	LIBRARY DESTINATION
		lib
)
