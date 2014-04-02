project(trabecula)

cmake_minimum_required(VERSION 2.6)


if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

#============= FIND EXTERNAL LIBRARIES ==========
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Qt4 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Eigen3 "3.0.3" REQUIRED)

set(QT_USE_QTOPENGL TRUE)
include(${QT_USE_FILE})

# =============== INCLUDES =======================
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories( ${QT_INCLUDE_DIR} )
include_directories( ${EIGEN3_INCLUDE_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/src )

add_definitions(-DTRABECULA_DIR="${PROJECT_SOURCE_DIR}")
add_definitions(-DEIGEN_DONT_ALIGN_STATICALLY)

if(APPLE)
  set(APPLE_WORKAROUND_SRCS include/core_profile_attributes.mm)
endif()

# =============== MAIN OBJECTS ===================
set(trabecula_SRCS
  src/cubesMesh.cpp
  src/camera.cpp
  src/renderingWidget.cpp
  src/shader.cpp
  src/main.cpp
  src/image3D.cpp
  src/main.cpp
  src/analyze_loader.cpp
  src/swap.cpp
  src/tubular_object.cpp
  src/scene.cpp
  ${APPLE_WORKAROUND_SRCS})

set (FOO_MOC_HEADERS include/renderingWidget.hpp)
qt4_wrap_cpp (FOO_MOC_OUTFILES ${FOO_MOC_HEADERS})

# Create an executable by just adding the main entry point
add_executable(trabecula ${trabecula_SRCS} ${FOO_MOC_OUTFILES})

# =============== LINK LIBRARIES =================
target_link_libraries(trabecula
  ${QT_QTCORE_LIBRARY}    ${QT_QTGUI_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}  ${OPENGL_LIBRARIES}
)

if(APPLE)
  target_link_libraries(trabecula "-framework Foundation" "-framework Cocoa")
endif()
