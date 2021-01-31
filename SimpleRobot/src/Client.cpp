#include "Client.h"

Client::Client(const char *serverIP, int serverPort_, int axies_, int buffer_size_) : serverPort(serverPort_), axies(axies_), bufferMaxSize(buffer_size_)
{
    
    std::cout << "客户端启动，尝试连接服务端  " << serverIP << ":" << serverPort_ << std::endl;
    // socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        std::cout << "Error: socket" << std::endl;
        exit(0);
    }

    //服务端 ip及程序端口号
    serverAddr.sin_family = AF_INET; //tcp IPv4
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);

    //尝试连接服务器
    if (connect(client_fd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << "Error: connect" << std::endl;
        exit(0);
    }

    //反馈数据结构
    feedback.state = STATE::STOPPED;                     //静止态
    feedback.axies = axies;
    feedback.pointSize = 2 + axies * 3;
    feedback.bufferMaxSize = 1 + feedback.pointSize;
    initComplexPointBuffer(feedback);


    //目标数据结构
    goal.state = STATE::STOPPED;                        //静止态
    goal.axies = axies;
    goal.pointSize = 2 + axies * 3;
    goal.pointsMaxCount = 400;
    goal.bufferMaxSize = bufferMaxSize;
    initTrajectoryBuffer(goal);
}

Client::~Client()
{
}

void Client::listening()
{
    while (MyFunctions::ok())
    {
        //路径点赋值
        goal.state = STATE::PENDING; //挂起态
        std::cout << "\n\nduration : ";
        cin >> goal.points[0].duration;
        for (int i = 0; i < goal.axies; i ++)
        {
            std::cout << "postions[" << i <<"] ";
            cin >> goal.points[0].postions[i];
        }
        goal.pointsCount = 1;
        trajectory2buffer(goal);
        printInfo(goal);
        printBuffer(goal);

        //发数据
        send(client_fd, goal.buffer, goal.bufferSize, 0);
        if (strcmp((char * )goal.buffer, "exit") == 0)
        {
            std::cout << "...disconnect" << std::endl;
            break;
        }
        usleep(period);
    }
    close(client_fd);
}

void Client::timerRegister(int period) //定时器初始化 us
{
    count = 0;
    this->period = period;
}