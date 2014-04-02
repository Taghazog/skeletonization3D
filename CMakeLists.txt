PROJECT(trabecula)

cmake_minimum_required(VERSION 2.8)


if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

# =============== INCLUDES =======================
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories( ${PROJECT_SOURCE_DIR}/src )
include_directories( ${PROJECT_SOURCE_DIR}/inc )

add_definitions(-DTRABECULA_DIR="${PROJECT_SOURCE_DIR}")

# =============== MAIN OBJECTS ===================
set(trabecula_SRCS 	main.cpp
				src/analyze_loader.cpp
				src/swap.cpp
				src/tubular_object.cpp)

add_executable(trabecula ${trabecula_SRCS})

# =============== LINK LIBRARIES =================