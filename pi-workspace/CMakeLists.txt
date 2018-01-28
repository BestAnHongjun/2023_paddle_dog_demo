cmake_minimum_required(VERSION 2.8.3)
project(unitree_legged_sdk)

include_directories(include)
link_directories(lib)

add_compile_options(-std=c++11)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(EXTRA_LIBS -pthread libunitree_legged_sdk_arm64.so lcm linux_udp)

set(CMAKE_CXX_FLAGS "-O3 -fPIC")

add_executable(follow main/follow.cpp)
target_link_libraries(follow ${EXTRA_LIBS})
