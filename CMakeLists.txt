cmake_minimum_required(VERSION 2.6)
project(ChattingRoom)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -Wall -g")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/lib)

add_subdirectory(src)

