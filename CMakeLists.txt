cmake_minimum_required(VERSION 3.11)
project(ionic)

set(MASTER_PROJECT OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(MASTER_PROJECT ON)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/ionic/*.h")

if (MASTER_PROJECT)
  set(SOURCES ${SOURCES} "test/test.cpp")
  install(TARGETS ionic EXPORT ionicTargets)
  
  enable_testing()

  add_test(
    NAME ionic
    COMMAND ionic
  )
  add_executable(ionic ${SOURCES} ${HEADERS})  
endif()

target_include_directories(ionic PUBLIC "include")


