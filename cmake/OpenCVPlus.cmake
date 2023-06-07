# Set include dir
include_directories(include)

# Set C++11 standard
add_definitions(-std=c++11)

# Make opencv library
find_package(OpenCV)
include_directories(${OpenCV_INCLUDE_DIRS})
find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})
add_library(opencv SHARED source/opencv.cpp)
target_link_libraries(opencv ${OpenCV_LIBS} ${JPEG_LIBRARIES})