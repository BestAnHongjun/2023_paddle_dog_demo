/*****************************************************************
 Copyright (c) 2020, Unitree Robotics.Co.Ltd. All rights reserved.
******************************************************************/

#include "unitree_legged_sdk/unitree_legged_sdk.h"
#include <math.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include "LinuxUdp.h"


using namespace UNITREE_LEGGED_SDK;

LinuxUdp::UdpServer udp_server(8900);

struct PACK
{
    float delta_w;
    float delta_h;
    int mode;
};

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
    int motiontime = 0;


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

bool run_flag = false;
int dance_cnt = 0;

void Custom::RobotControl() 
{
    PACK* pack;
    pack = new PACK;
    udp_server.UdpRecv((void*)pack);
    printf("mode:%d dw:%.2f dh:%.2f cnt:%d\n", pack->mode, pack->delta_w, pack->delta_h, dance_cnt);
    
    if (run_flag) return;
    run_flag = true;
    motiontime += 2;
    udp.GetRecv(state);

    cmd.mode = 0;      // 0:idle, default stand      1:forced stand     2:walk continuously
    cmd.gaitType = 0;
    cmd.speedLevel = 0;
    cmd.footRaiseHeight = 0;
    cmd.bodyHeight = 0;
    cmd.euler[0]  = 0;
    cmd.euler[1] = 0;
    cmd.euler[2] = 0;
    cmd.velocity[0] = 0.0f;
    cmd.velocity[1] = 0.0f;
    cmd.yawSpeed = 0.0f;
    cmd.reserve = 0;

    if(motiontime > 0 ){
        switch(pack->mode)
        {
        case -1:
            cmd.mode = 1;
            dance_cnt = 0;
            break;

        case 0:        
            cmd.mode = 2;
            cmd.gaitType = 1;
            cmd.velocity[0] = 0.05f;
            cmd.euler[1] = -pack->delta_h/200.0;
            cmd.euler[2] = pack->delta_w/300.0;
            cmd.yawSpeed = pack->delta_w/50.0;
            dance_cnt = 0;
            break;

        case 1:        
            dance_cnt += 1;
            if(dance_cnt == 500)
            {
                cmd.mode = 12;
                dance_cnt = 0;
            }
            break;
                           
        case 2:        
            cmd.mode = 1;
            cmd.euler[1] = -pack->delta_h/200.0;
            cmd.euler[2] = pack->delta_w/300.0;
            dance_cnt = 0;
            break;
        }
    }

    udp.SetSend(cmd); 
    delete pack;

    if (cmd.mode == 12)
    {
        usleep(15 * 1000 * 1000);
        printf("over.\n");
    }
    run_flag = false;
}

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
