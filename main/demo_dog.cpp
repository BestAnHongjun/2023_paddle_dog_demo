#include <unistd.h>
#include <stdlib.h>
#include <UnitreeCameraSDK.hpp>

#include "viz.h"
#include "paddle_api.h"
#include "edge_timer.h"
#include "udp_img_trans.h"
using namespace paddle::lite_api;

static const int INPUT_W = 416;
static const int INPUT_H = 416;


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
            blob[0 * img_w * img_h + h * img_w + w] = r;
            blob[1 * img_w * img_h + h * img_w + w] = g;
            blob[2 * img_w * img_h + h * img_w + w] = b;
        }
    }
}

int main(int argc, char *argv[]){
    if (argc != 3)
    {
        std::cout << "Usage: demo_dog <ip> <port>" << std::endl;
    }

    Timer timer_all("all");
    UDPImgSender img_sender_debug(argv[1], atoi(argv[2]));

    // init unitree camera
    int deviceNode = 1;
    cv::Size frameSize(1856, 800); ///< default frame size 1856x800
    int fps = 30; ///< default camera fps: 30
    UnitreeCamera cam(deviceNode); ///< init camera by device node number
    if(!cam.isOpened())   ///< get camera open state
        exit(EXIT_FAILURE);
    cam.setRawFrameSize(frameSize); ///< set camera frame size
    cam.setRawFrameRate(fps);       ///< set camera camera fps
    cam.setRectFrameSize(cv::Size(frameSize.width >> 2, frameSize.height >> 1)); ///< set camera rectify frame size
    cam.startCapture(); ///< disable image h264 encoding and share memory sharing
    
    // init predictor
    MobileConfig config;
    config.set_model_from_file("shared/yolox_int16.nb");
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
    while(cam.isOpened()){
        timer_all.start();
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

        // viz
        for (int i = 0; i < num_data[0]; i++)
        {
            const float* res = res_data + i * 6;
            int cls = int(res[0]);
            float score = res[1];
            float x1 = res[2];
            float y1 = res[3];
            float x2 = res[4];
            float y2 = res[5];
            if (score < 0.5) continue;
            printf("cls:%d score:%.2f x1:%.2f y1:%.2f x2:%.2f y2:%.2f\n", cls, score, x1, y1, x2, y2);
            viz(img, cls, score, x1, y1, x2 - x1, y2 - y1);
        }
        if (argc == 3)
        {
            img_sender_debug.send(img);
        }
        timer_all.end();
    }
    
    cam.stopCapture(); ///< stop camera capturing
    
    return 0;
}
