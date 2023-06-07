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
#ifndef UDP_IMG_TRANS_H
#define UDP_IMG_TRANS_H

#include "udp.h"
#include "opencv.h"


#define MODE_CV     0
#define MODE_JPGBUF 1
#define MODE_RGBBUF 2

class UDPImgSender
{
private:
    udp::UDPSender* udp_sender;
    char ip[16];
    uint16_t port;

    bool config;
    int mode;

public:
    UDPImgSender();
    UDPImgSender(const char* ip, uint16_t port, int mode=MODE_CV);
    ~UDPImgSender();
    
    void set_mode(int mode);
    void set_target_ip(const char* ip);
    void set_target_port(uint16_t port);
    bool init();

    void send(cv::Mat img);
    void sendjpg(uint8_t* jpg_buffer, size_t size);
    void sendrgb(uint8_t* rgb_buffer, size_t width, size_t height, int quality=95);
};

class UDPImgReceiver
{
private:
    udp::UDPReceiver* udp_receiver;
    uint16_t port;

    bool recv_handle;
    void recv_func();
    std::thread recv_thread;

    BlockingQueue<cv::Mat>* cv_que;
    BlockingQueue<std::vector<uint8_t> >* jpg_que;
    BlockingQueue<RgbImg>* rgb_que;

    bool config;
    int mode;

public:
    UDPImgReceiver();
    UDPImgReceiver(uint16_t port, int mode=MODE_CV);
    ~UDPImgReceiver();

    void set_mode(int mode);
    void set_listen_port(uint16_t port);
    bool init();

    cv::Mat read();
    std::vector<uint8_t> readjpg();
    RgbImg readrgb();
};

#endif // UDP_H

