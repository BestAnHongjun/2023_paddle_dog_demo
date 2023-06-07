# include UDPPlus
include(cmake/UDPPlus.cmake)

# include OpenCVPlus
include(cmake/OpenCVPlus.cmake)

# Make UDPImgTrans library
add_library(img_trans SHARED source/udp_img_trans.cpp)
target_link_libraries(img_trans udp opencv)
