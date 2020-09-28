#ifndef SERVER_H
#define SERVER_H

#include "Protocol.h"

class Server
{
private:
    int listenfd;               //tcp对象实例
    int connection_fd;          //连接对象
    bool isConnected;           //是否连接  true已连接  false未连接

    const int port;             //本程序(服务端)端口号

    char * serverIP;                 //本程序(服务端)IP
    struct sockaddr_in serverAddr;  //协议族
    socklen_t serverAddrLen;        //地址长度
    

    char * clientIP;                //客户端IP
    struct sockaddr_in clientAddr; //客户地址端口信息
    socklen_t clientAddrLen;       //地址长度

    const int bufferMaxSize; //缓存区大小设置
    //char *buffer;            //tcp通讯缓存区

    //timer
    int period;
    int count;

    const int axies; //机械臂轴数

public:
    Server(int port_ = 8000, int bufferMaxSize_ = 0x2000, int axies_ = 6); //默认8192

    ~Server();

    void listening(); //启动服务器，并接收数据

    Trajectory goal;           //目标轨迹
    ComplexPoint feedback;     //反馈当前位置
};

#endif //SERVER_H