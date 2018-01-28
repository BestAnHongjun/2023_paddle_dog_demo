//reference:https://blog.51cto.com/u_15127617/4311117
#ifndef ___UART_H___
#define ___UART_H___


#include <stdio.h>      /*标准输入输出定义*/
#include <stdlib.h>     /*标准函数库定义*/
#include <iostream>
#include <termios.h>
#include <errno.h>   
#include <time.h>
#include <string.h>

#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include <unistd.h>
#include <fcntl.h>

#include <thread>


// #define DEFAULT_UART_COM            "/dev/ttyS3" //环境探测器
// #define DEFAULT_VMIN                5
// #define DEFAULT_VTIME               6

class UART
{
private:
    int fd;
    // 串口打开关闭与读写
    int init(const char* path, int speed);
    //设置串口波特率
    int setSpeed(int speed);
    //设置串口的数据位、停止位、校验位。
    int setParity(int databits, int stopbits, char parity);

    unsigned char buffer[1024];
    int rec_size;
    void (*callback_func) (unsigned char*, int);

    bool recv_handle;
    std::thread recv_thread;
    void recv_func();

public:
    UART(const char* path, int speed, 
    int databits, int stopbits, char parity, void (*callback) (unsigned char*, int) = NULL, int recv_size=50, bool block=false);
    ~UART();

    //串口写入数据
    int send(const void* data, size_t size);

    //读取串口数据
    int recv(void* data, size_t size);
    
    //关闭串口
    int release();

// private:
//     static SerialPort* m_instance_ptr; //单例变量

// public:
//     static pthread_mutex_t mutex;
//     static SerialPort* getInstance(); //单例实例化函数
   
};


#endif // !___UART_H___