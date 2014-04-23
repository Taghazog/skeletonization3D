PROJECT(trabecula)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

#================ CONFIGURATION FILES ============

# Test if 64 bits or 32 bits OS.
set(UINT uint32_t)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(UINT uint64_t)
endif()

configure_file(inc/trabecula/build_info.hpp.in build_info.hpp)

# =============== INCLUDES =======================
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(src inc ${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DTRABECULA_DIR="${PROJECT_SOURCE_DIR}")

# =============== MAIN OBJECTS ===================
set(trabecula_SRCS 	main.cpp
				src/analyze_loader.cpp
				src/swap.cpp
				src/tubular_object.cpp)

add_executable(trabecula ${trabecula_SRCS})

# =============== LINK LIBRARIES =================