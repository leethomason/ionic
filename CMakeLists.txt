cmake_minimum_required(VERSION 3.11)
project(ionic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

if (PROJECT_IS_TOP_LEVEL)
  message("Top level project")
endif()

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/ionic/*.h")

add_library(ionic STATIC)
target_sources(ionic PRIVATE ${SOURCES} ${HEADERS})

target_include_directories(ionic
    PRIVATE
        # where the library itself will look for its internal headers
        "include"
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

enable_testing()

add_test(
  NAME ionic_test
  COMMAND ionic_test
)

add_executable(ionic_test "test/test.cpp")  
target_link_libraries(ionic_test ionic)

