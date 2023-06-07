#ifndef __VIZ_H__
#define __VIZ_H__


#include <opencv2/opencv.hpp>

void viz(cv::Mat &img, int cls, float score, float x, float y, float w, float h);

#endif