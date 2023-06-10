# Nano1视觉模型部署教程
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
#include "unitree_legged_sdk/unitree_legged_sdk.h"
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "LinuxUdp.h"

using namespace UNITREE_LEGGED_SDK;
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