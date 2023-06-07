# Set include dir
include_directories(include)

# Set C++11 standard
add_definitions(-std=c++11)

# Make udp library
add_library(udp SHARED source/network.cpp source/udp_sender.cpp source/udp_receiver.cpp)
target_link_libraries(udp pthread)