#include "Server.h"

Server::Server(int port_, int bufferMaxSize_, int axies_) : port(port_), bufferMaxSize(bufferMaxSize_), axies(axies_)
{
    serverIP = new char [INET_ADDRSTRLEN];
    serverAddrLen = (socklen_t)sizeof(serverAddr);
    clientIP = new char[INET_ADDRSTRLEN];
    clientAddrLen = (socklen_t)sizeof(clientAddr);
    inet_ntop(AF_INET, &(serverAddr.sin_addr), serverIP, INET_ADDRSTRLEN);
    std::cout << "信息 : 服务端信息 " << serverIP << ":" << port << std::endl;
    std::cout << "信息 : 服务器已启动" << std::endl;

    isConnected = false;
    // socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd == -1)
    {
        std::cout << "Error: socket" << std::endl;
        exit(0);
    }

    // bind
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;



    if (bind(listenfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cout << "Error: bind， 服务端口已被占用" << std::endl;
        exit(0);
    }

    // listen
    if (listen(listenfd, 5) == -1)
    {
        std::cout << "Error: listen" << std::endl;
        exit(0);
    }
    else
    {
        std::cout << "信息 : 监听设置完毕" << std::endl;
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

Server::~Server()
{
}

void Server::listening()
{
    while (MyFunctions::ok())
    {
        std::cout << "信息 : 服务器等待连接" << std::endl;

        connection_fd = accept(listenfd, (struct sockaddr *)&clientAddr, &clientAddrLen);

        if (connection_fd < 0)
        {
            if (MyFunctions::ok())
            {
                std::cout << "Error: 服务器端口连接启动失败" << std::endl;
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            isConnected = true;
        }
        if (isConnected)
        {
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            std::cout << "信息 : 客户端信息 " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
        }
        
        while (MyFunctions::ok())
        {
            //memset(buffer, 0, bufferMaxSize);

            //收数据
            int len = recv(connection_fd, goal.buffer, goal.bufferMaxSize, 0); //阻塞态

            //退出连接
            if (strcmp((char *)goal.buffer, "exit") == 0 || len == 0)
            {
                std::cout << "信息 : 客户端断开连接 " << clientIP << ":" << ntohs(clientAddr.sin_port) << std::endl;
                break;
            }

            //获取缓存区长度
            goal.bufferSize = len;

            buffer2trajectory(goal);
            printTrajectory(goal);
        }
        
        close(connection_fd);
        isConnected = false;
        usleep(period);
    }
    close(listenfd);
}