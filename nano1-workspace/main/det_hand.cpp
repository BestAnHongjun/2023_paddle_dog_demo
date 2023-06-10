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
#include <unistd.h>
#include <stdlib.h>
#include <UnitreeCameraSDK.hpp>

#include "viz.h"
#include "opencv.h"
#include "LinuxUdp.h"
#include "paddle_api.h"

using namespace LinuxUdp;
using namespace paddle::lite_api;

static const int INPUT_W = 320;
static const int INPUT_H = 320;

struct PACK
{
    float delta_w;
    float delta_h;
    int mode;
};

int64_t ShapeProduction(const shape_t& shape) {
    int64_t res = 1;
    for (auto i : shape) res *= i;
    return res;
}

void blobFromImage(cv::Mat& img, float* blob){
    int img_h = img.rows;
    int img_w = img.cols;
    for (size_t  h = 0; h < img_h; h++) 
    {
        for (size_t w = 0; w < img_w; w++) 
        {
            cv::Vec3b pix = img.at<cv::Vec3b>(h, w);
            float r = (float)pix[0];
            float g = (float)pix[1];
            float b = (float)pix[2];
            r = ((r / 255.0) - 0.485) / 0.229;
            g = ((g / 255.0) - 0.456) / 0.224;
            b = ((b / 255.0) - 0.406) / 0.225;
            blob[0 * img_w * img_h + h * img_w + w] = r;
            blob[1 * img_w * img_h + h * img_w + w] = g;
            blob[2 * img_w * img_h + h * img_w + w] = b;
        }
    }
}

int main(int argc, char *argv[]){
    UdpClient udp_client("192.168.123.161", 8900);

    // init unitree camera
    int deviceNode = 1;
    cv::Size frameSize(928, 400); ///< default frame size 1856x800
    int fps = 100; ///< default camera fps: 30
    UnitreeCamera cam(deviceNode); ///< init camera by device node number
    if(!cam.isOpened())   ///< get camera open state
        exit(EXIT_FAILURE);
    cam.setRawFrameSize(frameSize); ///< set camera frame size
    cam.setRawFrameRate(fps);       ///< set camera camera fps
    cam.setRectFrameSize(cv::Size(frameSize.width >> 2, frameSize.height >> 1)); ///< set camera rectify frame size
    cam.startCapture(); ///< disable image h264 encoding and share memory sharing
    
    // init predictor
    MobileConfig config;
    config.set_model_from_file("shared/voc_hand.nb");
    config.set_power_mode(static_cast<paddle::lite_api::PowerMode>(0));
    config.set_threads(4);
    std::shared_ptr<PaddlePredictor> predictor = CreatePaddlePredictor<MobileConfig>(config);
    std::unique_ptr<Tensor> image_tensor(std::move(predictor->GetInputByName("image")));
    std::unique_ptr<Tensor> scale_tensor(std::move(predictor->GetInputByName("scale_factor")));
    image_tensor->Resize({1, 3, INPUT_H, INPUT_W});
    scale_tensor->Resize({1, 2});
    auto* image_data = image_tensor->mutable_data<float>();
    auto* scale_data = scale_tensor->mutable_data<float>();
    scale_data[1] = INPUT_W / ((frameSize.width >> 2) * 1.0);
    scale_data[0] = INPUT_H / ((frameSize.height >> 1) * 1.0);


    usleep(500000);
    std::vector<uint8_t> buffer;
    cv::Mat img, rgb_img, pr_img;

    PACK pack;

    while(cam.isOpened()){
        // read img
        cv::Mat left,right;
        if(!cam.getRectStereoFrame(left,right)){ ///< get rectify left,right frame  
            usleep(1000);
            continue;
        }
        cv::flip(left, img, -1);

        // preprocess
        cv::cvtColor(img, rgb_img, cv::COLOR_BGR2RGB);
        cv::resize(rgb_img, pr_img, cv::Size(INPUT_W, INPUT_H));
        blobFromImage(pr_img, image_data);

        // predict
        predictor->Run();

        // postprocess
        std::unique_ptr<const Tensor> res_tensor(std::move(predictor->GetTensor("multiclass_nms3_0.tmp_0")));
        std::unique_ptr<const Tensor> num_tensor(std::move(predictor->GetTensor("multiclass_nms3_0.tmp_2")));
        auto res_data = res_tensor->data<float>();
        auto num_data = num_tensor->data<int>();
        std::cout << num_data[0] << std::endl;

        int max_cls = -1;
        float max_area = 0;
        float cx, cy;

        // viz
        for (int i = 0; i < num_data[0]; i++)
        {
            const float* res = res_data + i * 6;
            int cls = int(res[0]);
            if (cls == 3) continue; // no_gesture
            float score = res[1];
            if (score < 0.6) continue;
            float x1 = res[2];
            float y1 = res[3];
            float x2 = res[4];
            float y2 = res[5];
            float area = (x2 - x1) * (y2 - y1);
            if (area > max_area)
            {
                cx = (x1 + x2) / 2.0;
                cy = (y1 + y2) / 2.0;
                max_area = area;
                max_cls = cls;
            }
            printf("cls:%d score:%.2f x1:%.2f y1:%.2f x2:%.2f y2:%.2f\n", cls, score, x1, y1, x2, y2);
            viz(img, cls, score, x1, y1, x2 - x1, y2 - y1);
        }

        if (max_cls != -1)
        {
            pack.delta_w = (frameSize.width >> 3) - cx;
            pack.delta_h = (frameSize.height >> 2) - cy;
            if (max_cls == 0)
            {   // 666
                pack.mode = 1;  // 跳舞
                pack.delta_w = 0;
                pack.delta_h = 0;
            }
            else if (max_cls == 1)
            {   // like
                pack.mode = 0;  // 跟随
            }
            else if (max_cls == 2)
            {   // stop
                pack.mode = 2;  // 停止
            }
        }
        else 
        {
            pack.mode = -1;
            pack.delta_w = 0;
            pack.delta_h = 0;
        }
        printf("mode:%d delta_w:%.2f delta_h:%.2f\n", pack.mode, pack.delta_w, pack.delta_h);
        udp_client.UdpSend((void*)&pack, sizeof(PACK));

        // cv::imshow("demo", img);
        // cv::waitKey(5);
    }
    
    cam.stopCapture(); ///< stop camera capturing
    
    return 0;
}
