# 中国软件杯飞桨小汪线下实训营-Go1Demo

项目主页：[AI Studio](https://aistudio.baidu.com/aistudio/projectdetail/6112816)

# 一、模型训练教程
请参考[项目主页](https://aistudio.baidu.com/aistudio/projectdetail/6112816)。

# 二、模型部署教程
* 克隆本项目至机器狗Nano1和树莓派home目录。
```sh
cd ~
git clone https://gitee.com/an_hongjun/2023_paddle_dog_demo.git
```
## 1.Nano1部署
* 进入nano1-workspace目录
```sh
cd nano1-workspace
```
* 杀进程
* 运行nano1节点
```
./bin/det_hand
```
## 2.树莓派部署
* 进入pi-workspace目录
```sh
cd pi-workspace
```
* 运行pi节点
```
./bin/follow
```

# 三、代码解释
* [Nano1节点代码解释](./nano1-workspace/README.md)
* [Pi节点代码解释](./pi-workspace/README.md)

### 贡献者：
* 安泓郡（Coder.AN, an.hongjun@foxmail.com）
>个人简介：大连海事大学智能科学与技术专业大三在读。 研究方向：机器视觉、智能自主机器人。 RoboCup 机器人世界杯中国赛水下作业组全国总冠军；全国大学生RoboCom 机器人开发者大赛物资运送赛道全国总冠军；中国大学生计算机设计大赛飞桨赛道全国总冠军；全国大学生嵌入式芯片与系统设计竞赛海思赛道全国总冠军。 保研至西北工业大学光电与智能研究院攻读博士研究生。
* 莫善彬（Pomeloo, Bingger_0619@dlmu.edu.cn）
>个人简介：我的名字是莫善彬，是来自大连海事大学的一个喜欢玩机器控制的boy，专业是电子信息科学与技术。主攻机器视觉以及硬件机器控制方向，热爱智能化以及AI。在各类大赛中都有涉猎，同时我也是2022届ROBCOM物资运送的冠军队伍选手，熟悉四足机器狗的控制逻辑及其底层运动控制，有一定的开发经验。其余参加的大多数比赛都是偏向于硬件的比赛，比如全国大学生电子设计大赛，蓝桥杯等一些嵌入式硬件开发，屡次夺得佳绩。
### 鸣谢单位：
* 大连海事大学人工智能学院（[欢迎报考](https://ai.dlmu.edu.cn/)）
* 飞桨PaddlePaddle-源于产业实践的开源深度学习平台
* 宇树科技—全球四足机器人行业开创者