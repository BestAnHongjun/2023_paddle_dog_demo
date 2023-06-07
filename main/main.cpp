#include <string>
#include <cstdio>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "viz.h"
#include "paddle_api.h"
using namespace paddle::lite_api;


static const int INPUT_W = 416;
static const int INPUT_H = 416;


int64_t ShapeProduction(const shape_t& shape) {
  int64_t res = 1;
  for (auto i : shape) res *= i;
  return res;
}

cv::Mat static_resize(cv::Mat& img) {
  float r = std::min(INPUT_W / (img.cols*1.0), INPUT_H / (img.rows*1.0));
  // r = std::min(r, 1.0f);
  int unpad_w = r * img.cols;
  int unpad_h = r * img.rows;
  cv::Mat re(unpad_h, unpad_w, CV_8UC3);
  cv::resize(img, re, re.size());
  cv::Mat out(INPUT_H, INPUT_W, CV_8UC3, cv::Scalar(0, 0, 0));
  re.copyTo(out(cv::Rect(0, 0, re.cols, re.rows)));
  return out;
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
      // r = ((r / 255.0) - 0.485) / 0.229;
      // g = ((g / 255.0) - 0.456) / 0.224;
      // b = ((b / 255.0) - 0.406) / 0.225;
      blob[0 * img_w * img_h + h * img_w + w] = r;
      blob[1 * img_w * img_h + h * img_w + w] = g;
      blob[2 * img_w * img_h + h * img_w + w] = b;
    }
  }
}

int main()
{
  cv::Mat img, rgb_img, pr_img;
  img = cv::imread("shared/test2.jpg");
  cv::cvtColor(img, rgb_img, cv::COLOR_BGR2RGB);
  int img_w = img.cols;
  int img_h = img.rows;
  cv::resize(rgb_img, pr_img, cv::Size(416, 416));

  MobileConfig config;
  config.set_model_from_file("shared/yolox.nb");
  std::shared_ptr<PaddlePredictor> predictor = CreatePaddlePredictor<MobileConfig>(config);

  std::unique_ptr<Tensor> image_tensor(std::move(predictor->GetInputByName("image")));
  std::unique_ptr<Tensor> scale_tensor(std::move(predictor->GetInputByName("scale_factor")));

  image_tensor->Resize({1, 3, 416, 416});
  scale_tensor->Resize({1, 2});

  auto* image_data = image_tensor->mutable_data<float>();
  auto* scale_data = scale_tensor->mutable_data<float>();

  blobFromImage(pr_img, image_data);
  scale_data[1] = INPUT_W / (img.cols * 1.0);
  scale_data[0] = INPUT_H / (img.rows * 1.0);

  printf("%d %d\n", img.rows, img.cols);
  printf("%.2f %.2f\n", scale_data[0], scale_data[1]);

  predictor->Run();

  std::unique_ptr<const Tensor> res_tensor(std::move(predictor->GetTensor("multiclass_nms3_0.tmp_0")));
  std::unique_ptr<const Tensor> num_tensor(std::move(predictor->GetTensor("multiclass_nms3_0.tmp_2")));
  
  // 转化为数据 
  auto res_data = res_tensor->data<float>();
  auto num_data = num_tensor->data<int>();

  std::cout << num_data[0] << std::endl;
  for (int i = 0; i < num_data[0]; i++)
  {
    const float* res = res_data + i * 6;
    int cls = int(res[0]);
    float score = res[1];
    float x1 = res[2];
    float y1 = res[3];
    float x2 = res[4];
    float y2 = res[5];

    if (score < 0.2) continue;
    printf("cls:%d score:%.2f x1:%.2f y1:%.2f x2:%.2f y2:%.2f\n", cls, score, x1, y1, x2, y2);
    viz(img, cls, score, x1, y1, x2 - x1, y2 - y1);
  }

  cv::imwrite("output.jpg", img);

  return 0;
}