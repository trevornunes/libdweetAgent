project (dweetAgent)
cmake_minimum_required (VERSION 2.8)
include_directories(include)
add_library(dweetAgent src/cJSON.c src/dweetApi.cpp src/dweet.c)
install(TARGETS dweetAgent DESTINATION lib)
install(FILES include/dweet.h DESTINATION include)
install(FILES include/dweetApi.hpp DESTINATION include)

IF (CMAKE_COMPILER_IS_GNUCC)
	set(CMAKE_CXX_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
	set(CMAKE_C_FLAGS "-g -O0 -Wall -fprofile-arcs -ftest-coverage")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage -lgcov")
ENDIF()

target_include_directories (dweetAgent PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# Some example binaries to use the above library.
add_subdirectory(Examples)


