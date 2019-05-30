cmake_minimum_required(VERSION 3.9)
project(slic_saliency_test VERSION 1.0.0)

include(GNUInstallDirs)

## System dependencies are found with CMake's conventions
find_package(Boost REQUIRED COMPONENTS system filesystem thread chrono)
# Set your opencv directory
set(Caffe_DIR /usr/libs/caffe/build)
find_package(Caffe REQUIRED NO_DEFAULT_PATH)

find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs)
if(NOT OpenCV_FOUND) # if not OpenCV 3.x, then imgcodecs are not found
  find_package(OpenCV REQUIRED COMPONENTS core highgui imgproc)
endif()
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
list(APPEND Caffe_LINKER_LIBS ${OpenCV_LIBS})
message(STATUS "OpenCV found (${OpenCV_CONFIG_PATH})")
add_definitions(-DUSE_OPENCV)

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} -g -lm -fPIC -std=c++11")
include_directories(
  ./caffe/build/include
  ./include
  ./create_fill_gap/src/include
  ./create_fill_gap/src/lbe
  ./create_fill_gap/src/slic
  ${Caffe_INCLUDE_DIRS}
)

add_library(rgbd_saliency SHARED src/rgbdsaliency.cpp ./src/objectseg.cpp ./src/pointseg.cpp ./src/facade.cpp ./src/configprop.cpp ./create_fill_gap/src/fillgapcalc.cpp ./create_fill_gap/src/lbe/lbe.cpp ./create_fill_gap/src/lbe/superpixels.cpp ./create_fill_gap/src/slic/SLIC.cpp)

target_link_libraries(rgbd_saliency ${Caffe_LIBRARIES} ${Boost_LIBRARIES} ${OpenCV_LIBS})

set(executable get_results)

## Declare a C++ executable
add_executable(${executable}.bin src/test_main.cpp)
set_target_properties(${executable}.bin PROPERTIES OUTPUT_NAME ${executable})
target_link_libraries(${executable}.bin rgbd_saliency ${Caffe_LIBRARIES} ${Boost_LIBRARIES} ${OpenCV_LIBS})
