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
#ifndef __LINUX_UDP_H__
#define __LINUX_UDP_H__

#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <unistd.h>   
#include <errno.h>   
#include <string.h>   
#include <stdlib.h>
#include <thread>
#include <mutex>  
#include <arpa/inet.h>

// reference: https://www.jianshu.com/p/6c065429d3c1

namespace LinuxUdp {    // namespace LinuxUdp

#define MAX_BUFF_LEN 4096

/* error code */
#define SUCCESS                 0
#define ERROR_CREATE_SOCKET     -1
#define ERROR_BIND_SOCKET       -2

/* UDP Server */
class UdpServer {
private:
    int socket_fd;
    
    int recv_buff_len;
    unsigned char recv_buff[MAX_BUFF_LEN];
    std::mutex recv_mutex;

    bool run_recv_thread;
    std::thread recv_thread;
    void recv_handle(); // 异步接收数据线程
public:
    /* 启动UDP服务器
        @param: port, 监听端口
        @param: error_code, 错误代码
    */
    UdpServer(int port);

    /* 接收UDP包
        @param: buffer, 用户接收缓冲区
        @return: 成功接收字节长度
    */
    int UdpRecv(void* buffer);

    /* 关闭UDP服务器 */
    void UdpClose();
    ~UdpServer();
};

/* UDP Client */
class UdpClient {
private:
    int socket_fd;
    struct sockaddr_in addr_serv;

    int send_buff_len;
    unsigned char send_buff[MAX_BUFF_LEN];
public:
    /* 启动UDP客户端
        @param: ip_addr, 目标服务器ipv4地址
        @param: port，目标服务器端口
    */
    UdpClient(std::string ip_addr, int port);

    /* 发送UDP包
        @param: buffer，用户发送缓冲区
        @param: len, 发送字节长度
        @return: 成功发送字节长度
    */
    int UdpSend(void* buffer, int len);

    /* 关闭UDP服务器 */
    void UdpClose();
    ~UdpClient();
};

}   // namespace LinuxUdp

#endif