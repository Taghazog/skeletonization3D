cmake_minimum_required(VERSION 2.8)


if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories( ${PROJECT_SOURCE_DIR}/src )
include_directories( ${PROJECT_SOURCE_DIR}/inc )


# the list of source files
set(ped_SRCS src/main.cpp src/analyze_loader.cpp src/swap.cpp src/tubular_object.cpp)

add_executable(ped ${ped_SRCS})
