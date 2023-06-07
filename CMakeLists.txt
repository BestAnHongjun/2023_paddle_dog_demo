cmake_minimum_required(VERSION 2.8)

link_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/include)

set(CMAKE_C_COMPILER "aarch64-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "aarch64-linux-gnu-g++")

message(STATUS "armlinux CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "armlinux CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_definitions(-std=c++11 -O3 -pthread)

find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})

include(cmake/EdgeTimer.cmake)
include(cmake/UDPImgTrans.cmake)

# set(SDKLIBS unitree_camera tstc_V4L2_xu_camera udev systemlog ${OpenCV_LIBS})

# add_executable(demo_dog main/demo_dog.cpp source/viz.cpp)
# target_link_libraries(demo_dog ${SDKLIBS} opencv img_trans edge_timer)
# target_link_libraries(demo_dog -lpaddle_light_api_shared)
# target_link_libraries(demo_dog -ldl)
# target_link_libraries(demo_dog ${OpenCV_LIBS})

add_executable(main main/main.cpp source/viz.cpp)
target_link_libraries(main -lpaddle_light_api_shared)
target_link_libraries(main -ldl)
target_link_libraries(main edge_timer)
target_link_libraries(main ${OpenCV_LIBS})
