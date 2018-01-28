/*
 * Copyright (C) 2023 Coder.AN
 * Email: an.hongjun@foxmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef __OPENCV_H__
#define __OPENCV_H__


#include <stdint.h>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgcodecs/legacy/constants_c.h>

#include <jpeglib.h>

void MatToJpgBuffer(const cv::Mat mat, std::vector<uint8_t>& buff, int quality=95);
void JpgBufferToMat(uint8_t* buff, uint32_t size, cv::Mat &mat);

struct RgbImg
{
    size_t width;
    size_t height;
    std::vector<uint8_t> buffer;
};

RgbImg JpgBufferToRgbBuffer(uint8_t* jpg_buffer, size_t jpg_size);
std::vector<uint8_t> RgbBufferToJpgBuffer(uint8_t* rgb_buffer, size_t width, size_t height, int quality);

#endif // __OPENCV_H__