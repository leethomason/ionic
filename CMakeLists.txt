cmake_minimum_required(VERSION 3.11)
include(FetchContent)
project(ionic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG        e57ca2e3685b160617d3d95fcd9e789c4e06ca88 # 10.1.0
)

FetchContent_MakeAvailable(fmt)

file(GLOB SOURCES "src/*.cpp" "test/test.cpp")
file(GLOB HEADERS "include/ionic/*.h")

add_executable(ionic ${SOURCES} ${HEADERS} ${fmt_SOURCE_DIR}/src/format.cc)
target_include_directories(ionic PUBLIC "include" ${fmt_SOURCE_DIR}/include)



