# Set include dir
include_directories(include)

# Set C++11 standard
add_definitions(-std=c++11)

# Make opencv library
add_library(edge_timer SHARED source/edge_timer.cpp)