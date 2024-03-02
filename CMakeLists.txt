cmake_minimum_required(VERSION 3.11)
project(ionic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/ionic/*.h")

set(SOURCES ${SOURCES} "test/test.cpp")

enable_testing()

add_test(
  NAME ionic
  COMMAND ionic
)
add_executable(ionic ${SOURCES} ${HEADERS})  

target_include_directories(ionic PUBLIC "include")


