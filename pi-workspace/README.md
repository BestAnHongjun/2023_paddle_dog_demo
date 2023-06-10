# 关于结合目标检测的运动控制逻辑-Raspi代码
## 一、项目目录
* bin: 存放编译好的二进制可执行文件
* include: 存放C++头文件
* lib: 存放编译好的动态链接库
* main: 主程序代码(**关键**)
* build.sh: 编译脚本
* CMakeLists.txt: CMake文件

## 二、主程序代码讲解
[follow.cpp](main/follow.cpp)
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

* UDP的端口申明以及结构体定义
```cpp
LinuxUdp::UdpServer udp_server(8900); //从8900端口接收

struct PACK
{
    float delta_w;
    float delta_h;
    int mode;
};

bool run_flag = false;
int dance_cnt = 0;
```
* Custom中的高层控制以及底层UDP
```cpp
class Custom
{
public:
    Custom(uint8_t level): 
      safe(LeggedType::Go1), 
      udp(8090, "192.168.123.161", 8082, sizeof(HighCmd), sizeof(HighState))
    {
        udp.InitCmdData(cmd);
    }
    void UDPRecv();
    void UDPSend();
    void RobotControl();

    Safety safe;
    UDP udp;
    HighCmd cmd = {0};
    HighState state = {0};
    int motiontime = 0;   //系统计时时钟
    float dt = 0.002;     // 0.001~0.01
       
};

void Custom::UDPRecv()
{
    udp.Recv();
}

void Custom::UDPSend()
{  
    udp.Send();
}
```

### 2.2 Robcontrol-控制逻辑部分
* 主体逻辑
```cpp
 if(motiontime > 0 ){
        switch(pack->mode)    //目标检测后的不同模式传入
        {
        case -1:              //模式-1：原地不动
            cmd.mode = 1;
            dance_cnt = 0;
            break;
        case 0:               //模式0：实现跟随目标行动        
            cmd.mode = 2;
            cmd.gaitType = 1;
            cmd.velocity[0] = 0.05f;
            cmd.euler[1] = -pack->delta_h/200.0;
            cmd.euler[2] = pack->delta_w/300.0;
            cmd.yawSpeed = pack->delta_w/50.0;
            dance_cnt = 0;
            break;
        case 1:               //模式1：进入跳舞模式        
            dance_cnt += 1;
            if(dance_cnt == 500)
            {
                cmd.mode = 12;
                dance_cnt = 0;
            }
            break;               
        case 2:              //模式2：原地不动，仅实现头部跟随        
            cmd.mode = 1;
            cmd.euler[1] = -pack->delta_h/200.0;
            cmd.euler[2] = pack->delta_w/300.0;
            dance_cnt = 0;
            break;
        }
    }

    udp.SetSend(cmd); 
    delete pack;

    if (cmd.mode == 12)      //将跳舞在15s内完成，无其余操作
    {
        usleep(15 * 1000 * 1000);
        printf("over.\n");
    }
    run_flag = false;
```

### 2.3 main函数-进行loop循环
* 关于底层udp传输的loop循环
```cpp
int main(void) 
{
    std::cout << "Communication level is set to HIGH-level." << std::endl
              << "WARNING: Make sure the robot is standing on the ground." << std::endl
              << "Press Enter to continue..." << std::endl;
    std::cin.ignore();

    Custom custom(HIGHLEVEL);
    // InitEnvironment();
    LoopFunc loop_control("control_loop", custom.dt,    boost::bind(&Custom::RobotControl, &custom));
    LoopFunc loop_udpSend("udp_send",     custom.dt, 3, boost::bind(&Custom::UDPSend,      &custom));
    LoopFunc loop_udpRecv("udp_recv",     custom.dt, 3, boost::bind(&Custom::UDPRecv,      &custom));

    loop_udpSend.start();
    loop_udpRecv.start();
    loop_control.start();

    while(1)
    {
        sleep(10);
    };

    return 0; 
}
```

请[点击此处](main/follow.cpp)打开完整代码。

## 三、编译方法
* 修改main/follow.cpp和CMakeLists.txt
* 运行编译脚本进行编译。
```sh
./build.sh
```