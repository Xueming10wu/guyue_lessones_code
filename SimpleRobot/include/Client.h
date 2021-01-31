#ifndef CLIENT_H
#define CLIENT_H

#include "Protocol.h"

class Client
{
private:
    int client_fd;               //tcp对象实例
    struct sockaddr_in serverAddr; //协议族
    const int serverPort;          //服务程序端口号

    bool isConnected;  //是否连接  true已连接  false未连接

    const int bufferMaxSize; //缓存区大小设置
    //char *buffer;         //tcp通讯缓存区


    //timer
    int period;
    int count;

    const int axies; //机械臂轴数

public:
    Client(const char * serverIP = "127.0.0.1", int serverPort_ = 8000, int axies_ = 6, int buffer_size_ = 0x2000); //默认8192
    
    ~Client();

    void listening(); //启动服务器，并接收数据

    void timerRegister(int period = 100000); //定时器初始化 us

    Trajectory goal;     //目标轨迹
    ComplexPoint feedback; //反馈当前点
};

#endif //CLIENT_H