# Nano1视觉模型部署教程

请将你在AI Studio下载的模型文件放到shared文件下。

## 一、项目目录
* bin: 存放编译好的二进制可执行文件
* include: 存放C++头文件
* lib: 存放编译好的动态链接库
* main: 主程序代码(**关键**)
* shared: 存放其他文件(如模型文件)
* source: 存放除主程序外的其他代码
* build.sh: 编译脚本
* clean.sh: 用于清空编译时产生的build文件夹
* CMakeLists.txt: CMake文件
## 二、代码讲解
[det_hand.cpp](main/det_hand.cpp)
### 2.1 参数定义以及高层初始化定义

* 头文件
```cpp
#include <unistd.h>
#include <stdlib.h>
#include <UnitreeCameraSDK.hpp>

#include "viz.h"
#include "opencv.h"
#include "LinuxUdp.h"
#include "paddle_api.h"

using namespace LinuxUdp;
using namespace paddle::lite_api;
```
* UDP包结构体定义

检测到手势后，解析出其偏移画面中心点的水平量和垂直量，并把手势种类对应为mode，后期将pack包通过udp发送至树莓派。
```cpp
struct PACK
{
    float delta_w;   // 水平偏移量
    float delta_h;   // 垂直偏移量
    int mode;        // 模式
};
```
### 2.2 主函数讲解
下面看到main函数。
* 初始化udp发送端
  
首先定义udp发送端，目标设备ip地址为192.168.123.161，端口为8900.
```cpp
UdpClient udp_client("192.168.123.161", 8900);
```
* 初始化鱼眼相机
```cpp
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
```
* 初始化PaddleLite预测器
```cpp
// init predictor
MobileConfig config;
config.set_model_from_file("shared/voc_hand.nb");
config.set_power_mode(static_cast<paddle::lite_api::PowerMode>(0));
config.set_threads(4);
std::shared_ptr<PaddlePredictor> predictor = CreatePaddlePredictor<MobileConfig>(config);
// 下面两个输入节点的名字要和netron.app网站上看到的一致
std::unique_ptr<Tensor> image_tensor(std::move(predictor->GetInputByName("image")));
std::unique_ptr<Tensor> scale_tensor(std::move(predictor->GetInputByName("scale_factor")));
image_tensor->Resize({1, 3, INPUT_H, INPUT_W});
scale_tensor->Resize({1, 2});
auto* image_data = image_tensor->mutable_data<float>();
auto* scale_data = scale_tensor->mutable_data<float>();
scale_data[1] = INPUT_W / ((frameSize.width >> 2) * 1.0);
scale_data[0] = INPUT_H / ((frameSize.height >> 1) * 1.0);
```
关于PaddleLite预测器更加详细的使用方法，可以参考[PaddleLite文档-C++完整示例](https://www.paddlepaddle.org.cn/lite/v2.12/user_guides/cpp_demo.html)。

### 2.3 主循环
* 读入图像
```cpp
 // read img
cv::Mat left,right;
if(!cam.getRectStereoFrame(left,right)){ ///< get rectify left,right frame  
    usleep(1000);
    continue;
}
cv::flip(left, img, -1);
```
由unitree相机sdk读到的图像是倒着的，所以最后要flip翻转以下。
* 预处理图像

OpenCV读取的图像是BGR的，但PaddleLite模型接受的模型输入是RGB的，所以要做一下颜色空间的转换。

模型只接受固定输入320x320，所以要resize一下。

借助blobFromImage函数将cv::Mat转换成内存buffer，需要注意：(1)cv::Mat的内存中使用NHWC方式存储，而PaddleLite需要接受NCHW的输入；(2)原始RGB图像各像素点为uint8类型，需要转为float类型；(3)原始像素点是0~255之间的数，需要进行标准化。
```cpp
// preprocess
cv::cvtColor(img, rgb_img, cv::COLOR_BGR2RGB);
cv::resize(rgb_img, pr_img, cv::Size(INPUT_W, INPUT_H));
blobFromImage(pr_img, image_data);
```
* 使用PaddleLite进行推理

```cpp
// predict
predictor->Run();
```

* 取出推理结果

输出节点的名称也是和netron.app网站中看到的保持一致。
```cpp
// postprocess
std::unique_ptr<const Tensor> res_tensor(std::move(predictor->GetTensor("multiclass_nms3_0.tmp_0")));
std::unique_ptr<const Tensor> num_tensor(std::move(predictor->GetTensor("multiclass_nms3_0.tmp_2")));
auto res_data = res_tensor->data<float>();
auto num_data = num_tensor->data<int>();
std::cout << num_data[0] << std::endl;
```

* 过滤结果

```cpp
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
    viz(img, cls, score, x1, y1, x2 - x1, y2 - y1); // 顺便可视化
}
```

* 解析检测结果，发送指令到树莓派节点。
```cpp
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
```
请[点击此处](main/det_hand.cpp)打开完整代码。

## 三、编译方法
* 修改main/det_hand.cpp和CMakeLists.txt
* 运行编译脚本进行编译。
```sh
./build.sh
```