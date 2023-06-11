# 中国软件杯飞桨小汪线下实训营-Go1Demo

项目主页：[AI Studio](https://aistudio.baidu.com/aistudio/projectdetail/6112816)

## 预期目标：
* 使用mini-HaGRID数据集，借助PaddleDetection工具训练一个可以识别like、call、stop三种手势的目标检测模型

![](https://ai-studio-static-online.cdn.bcebos.com/51b752c5a1e64f88af002ee00638788a7a071aeb0553484699b8593544044cb8)

* 借助PaddleLite工具将检测模型部署至宇树Go1四足机器人
* 结合运动SDK，实现：
    * 没有检测到手势，小狗原地不动
    * 检测到like手势时，进行运动跟随
    
    ![](https://ai-studio-static-online.cdn.bcebos.com/9c8906d117104777ae9e0fc4daa560c1faadfe0f454d4d04896dfd8a9c5fffbf)
    
    * 检测到call手势时，小狗跳舞
    
    ![](https://ai-studio-static-online.cdn.bcebos.com/1b7f006cf91f4c3292123a3a8e98a90aedaaeb52e5b64438af42745786a95d6b)
    
    
    * 检测到stop手势时，原地静止跟随
    
    ![](https://ai-studio-static-online.cdn.bcebos.com/6455314b3e6d4df1af539b4fd90ed492985d3a3a2ce64d588edfbf1a69230121)
    
    

# 一、模型训练教程
请参考[项目主页](https://aistudio.baidu.com/aistudio/projectdetail/6112816)。

# 二、模型部署教程
* 克隆本项目至机器狗Nano1和树莓派home目录。
```sh
cd ~
git clone https://gitee.com/an_hongjun/2023_paddle_dog_demo.git
cd 2023_paddle_dog_demo
```
## 1.Nano1部署
* 进入nano1-workspace目录
```sh
cd nano1-workspace
```
* 杀进程
```sh
ps -aux | grep point_cloud_node | awk '{print $2}' | xargs kill -9
ps -aux | grep mqttControlNode | awk '{print $2}' | xargs kill -9
ps -aux | grep live_human_pose | awk '{print $2}' | xargs kill -9
ps -aux | grep rosnode | awk '{print $2}' | xargs kill -9
```
* 修改系统时间
将时间修改为你当前的时间。时间不对可能会影响CMake程序编译。
```sh
sudo date -s "2023-6-11 21:23:00"
```
* 编译程序
```sh
./build.sh
```
* 运行nano1节点
```
./bin/det_hand
```
## 2.树莓派部署
* 进入pi-workspace目录
```sh
cd pi-workspace
```
* 修改系统时间
将时间修改为你当前的时间。时间不对可能会影响CMake程序编译。
```sh
sudo date -s "2023-6-11 21:23:00"
```
* 编译程序
```sh
./build.sh
```
* 运行pi节点
```
./bin/follow
```

# 三、代码解释
* [Nano1节点代码解释](./nano1-workspace/README.md)
* [Pi节点代码解释](./pi-workspace/README.md)

<hr>

### 贡献者：
* 安泓郡（Coder.AN, an.hongjun@foxmail.com）
>个人简介：大连海事大学智能科学与技术专业大三在读。 研究方向：机器视觉、智能自主机器人。 RoboCup 机器人世界杯中国赛水下作业组全国总冠军；全国大学生RoboCom 机器人开发者大赛物资运送赛道全国总冠军；中国大学生计算机设计大赛飞桨赛道全国总冠军；全国大学生嵌入式芯片与系统设计竞赛海思赛道全国总冠军。 保研至西北工业大学光电与智能研究院攻读博士研究生。
* 莫善彬（Pomeloo, Bingger_0619@dlmu.edu.cn）
>个人简介：我的名字是莫善彬，是来自大连海事大学的一个喜欢玩机器控制的boy，专业是电子信息科学与技术。主攻机器视觉以及硬件机器控制方向，热爱智能化以及AI。在各类大赛中都有涉猎，同时我也是2022届ROBCOM物资运送的冠军队伍选手，熟悉四足机器狗的控制逻辑及其底层运动控制，有一定的开发经验。其余参加的大多数比赛都是偏向于硬件的比赛，比如全国大学生电子设计大赛，蓝桥杯等一些嵌入式硬件开发，屡次夺得佳绩。
### 鸣谢单位：
* 大连海事大学人工智能学院（[欢迎报考](https://ai.dlmu.edu.cn/)）
* 飞桨PaddlePaddle-源于产业实践的开源深度学习平台
* 宇树科技—全球四足机器人行业开创者