//参考 https://www.jianshu.com/p/3b233facd6bb
//函数手册(外网) https://man7.org/linux/man-pages/man2/open.2.html

#include "Protocol.h"

//应用数据结构转为流结构
void trajectory2buffer(Trajectory &trajectory_)
{
    //std::cout << "trajectory2buffer " << std::endl;
    //trajectory_.bufferSize = 1 + trajectory_.points.size() * trajectory_.pointSize;

    //记录缓存区使用长度
    trajectory_.bufferSize = 1 + trajectory_.pointSize * trajectory_.pointsCount;
    
    //设置轨迹状态
    trajectory_.buffer[0] = trajectory_.state;

    //对每个路径点进行访问
    for (int pointsSeq = 0; pointsSeq < trajectory_.pointsCount; pointsSeq++)
    {
        //std::cout <<"对每个路径点进行访问" << std::endl;
        //路径点执行所用时间高位
        trajectory_.buffer[1 + pointsSeq * trajectory_.pointSize] =
            (abs(trajectory_.points[pointsSeq].duration) >> 8) & 0xff;
        //路径点执行所用时间低位
        trajectory_.buffer[2 + pointsSeq * trajectory_.pointSize] =
            abs(trajectory_.points[pointsSeq].duration) & 0xff;

        //对路径点中的每个轴进行访问
        for (int axiesSeq = 0; axiesSeq < trajectory_.axies; axiesSeq++)
        {
            //std::cout <<"对路径点中的每个轴进行访问" << std::endl;
            //轴的符号位设置
            if (trajectory_.points[pointsSeq].postions[axiesSeq] < 0)
            {
                trajectory_.buffer[3 + pointsSeq * trajectory_.pointSize + axiesSeq * 3] = 0x0f;
            }
            else
            {
                trajectory_.buffer[3 + pointsSeq * trajectory_.pointSize + axiesSeq * 3] = 0xf0;
            }

            //数值位高位数据
            trajectory_.buffer[4 + pointsSeq * trajectory_.pointSize + axiesSeq * 3] =
                (abs(trajectory_.points[pointsSeq].postions[axiesSeq]) >> 8) & 0xff;

            //数值位低位数据
            trajectory_.buffer[5 + pointsSeq * trajectory_.pointSize + axiesSeq * 3] =
                abs(trajectory_.points[pointsSeq].postions[axiesSeq]) & 0xff;
        }
    }
}

void point2buffer(ComplexPoint & point_)
{
    //记录缓存区使用长度
    point_.bufferSize = 1 + point_.pointSize;

    //设置状态
    point_.buffer[0] = point_.state;

    //路径点执行所用时间高位
    point_.buffer[1] = (abs(point_.point.duration) >> 8) & 0xff;
    //路径点执行所用时间低位
    point_.buffer[2] = abs(point_.point.duration) & 0xff;

    //对路径点中的每个轴进行访问
    for (int axiesSeq = 0; axiesSeq < point_.axies; axiesSeq++)
    {
        //std::cout <<"对路径点中的每个轴进行访问" << std::endl;
        //轴的符号位设置
        if (point_.point.postions[axiesSeq] < 0)
        {
            point_.buffer[3 + axiesSeq * 3] = 0x0f;
        }
        else
        {
            point_.buffer[3 + + axiesSeq * 3] = 0xf0;
        }

        //数值位高位数据
        point_.buffer[4 + axiesSeq * 3] = (abs(point_.point.postions[axiesSeq]) >> 8) & 0xff;

        //数值位低位数据
        point_.buffer[5 + axiesSeq * 3] = abs(point_.point.postions[axiesSeq]) & 0xff;
    }
}


//流结构转为应用数据结构
void buffer2trajectory(Trajectory &trajectory_)
{
    //确定路径点的数量
    trajectory_.pointsCount = floor((trajectory_.bufferSize - 1) / trajectory_.pointSize);

    //获得轨迹状态
    trajectory_.state = trajectory_.buffer[0];

    //获取每个路径点
    for (int pointsSeq = 0; pointsSeq < trajectory_.pointsCount; pointsSeq++)
    {
        //获得每个路径点运动持续时间
        trajectory_.points[pointsSeq].duration =
            abs((int)((trajectory_.buffer[1 + pointsSeq * trajectory_.pointSize] << 8) |
                      trajectory_.buffer[2 + pointsSeq * trajectory_.pointSize]));

        //获取每个轴
        for (int axiesSeq = 0; axiesSeq < trajectory_.axies; axiesSeq ++)
        {
            //获得轴的数值
            trajectory_.points[pointsSeq].postions[axiesSeq] =
                abs((int)((trajectory_.buffer[4 + pointsSeq * trajectory_.pointSize + axiesSeq * 3] << 8) |
                          trajectory_.buffer[5 + pointsSeq * trajectory_.pointSize + axiesSeq * 3]));

            //获得轴的符号
            if (trajectory_.buffer[3 + pointsSeq * trajectory_.pointSize + axiesSeq * 3] == 0x0f)
            {
                trajectory_.points[pointsSeq].postions[axiesSeq] *= -1;
            }
        }
    }
}


void buffer2point(ComplexPoint & point_)
{
    //获得轨迹状态
    point_.state = point_.buffer[0];

    //获得运动持续时间
    point_.point.duration = abs((int)((point_.buffer[1] << 8) | point_.buffer[2]));

    //获取每个轴
    for (int axiesSeq = 0; axiesSeq < point_.axies; axiesSeq ++)
    {
        //获得轴的数值
        point_.point.postions[axiesSeq] =
            abs((int)((point_.buffer[4 + axiesSeq * 3] << 8) | point_.buffer[5 + axiesSeq * 3]));

        //获得轴的符号
        if (point_.buffer[3 + axiesSeq * 3] == 0x0f)
        {
            point_.point.postions[axiesSeq] *= -1;
        }
    }
}


/*
    uint8_t state;             //状态值，执行或取消
    int axies;                 //关节数

    int pointSize;             //单点缓存所占长度

    int bufferMaxSize;         //流缓存区最大长度
    int bufferSize;            //已使用的缓存区长度，用于通讯发送接收数据

    Point point;               //点
    uint8_t * buffer;          //流缓存区，用于对接socket
*/


//初始化缓存区
void initTrajectoryBuffer(Trajectory &trajectory_)
{
    //轨迹内存分配
    trajectory_.points = new Point[trajectory_.pointsMaxCount];

    //每个点都进行空间内存分配
    for (int i = 0; i < trajectory_.pointsMaxCount; i ++)
    {
        trajectory_.points[i].duration = 0;
        trajectory_.points[i].postions = new int[trajectory_.axies];
    }

    //通讯缓存区内存分配
    trajectory_.buffer = new uint8_t[trajectory_.bufferMaxSize];

    //已使用点数和缓存区数量
    trajectory_.pointsCount = 0;
    trajectory_.bufferSize = 0;
}

void initComplexPointBuffer(ComplexPoint & point_)
{
    //点内存分配
    point_.point.duration = 0;
    point_.point.postions = new int[point_.axies];

    //通讯缓存区内存分配
    point_.buffer = new uint8_t[point_.bufferMaxSize];

    //已使用缓存区数量
    point_.bufferSize = 0;
}



void printInfo(Trajectory &trajectory_)
{
    std::cout << "轨迹信息 : " << std::endl;
    std::cout << "状态 : " << trajectory_.state << std::endl;
    std::cout << "自由度 : " << trajectory_.axies << std::endl;
    std::cout << "单点缓存尺寸 : " << trajectory_.pointSize << std::endl;

    std::cout << "最大点数 : " << trajectory_.pointsMaxCount << std::endl;
    std::cout << "最大缓存区 : " << trajectory_.bufferMaxSize << " B"<< std::endl;

    std::cout << "已用点数 : " << trajectory_.pointsCount << std::endl;
    std::cout << "已使用的缓存区 : " << trajectory_.bufferSize << std::endl;
}


void printInfo(ComplexPoint &point_)
{
    std::cout << "当前点信息 : " << std::endl;
    std::cout << "状态 : " << point_.state << std::endl;
    std::cout << "自由度 : " << point_.axies << std::endl;
    std::cout << "单点缓存尺寸 : " << point_.pointSize << std::endl;

    std::cout << "最大缓存区 : " << point_.bufferMaxSize << "B"<< std::endl;
    std::cout << "已使用的缓存区 : " << point_.bufferSize << std::endl;
}



//打印轨迹信息
void printTrajectory(Trajectory &trajectory_)
{
    std::cout << "state : " << trajectory_.state << std::endl;
    for (int pointsSeq = 0; pointsSeq < trajectory_.pointsCount; pointsSeq++)
    {
        std::cout << "duration : " << trajectory_.points[pointsSeq].duration << ",   axies : ";
        for (int axiesSeq = 0; axiesSeq < trajectory_.axies; axiesSeq++)
        {
            std::cout << trajectory_.points[pointsSeq].postions[axiesSeq] << " ";
        }
        std::cout << dec << std::endl;
    }
}

//打印单点信息
void printComplexPoint(ComplexPoint &point_)
{
    std::cout << "duration : " << point_.point.duration << ",   axies : ";
    for (int axiesSeq = 0; axiesSeq < point_.axies; axiesSeq++)
    {
        std::cout << point_.point.postions[axiesSeq] << " ";
    }
    std::cout << dec << std::endl;
}


//打印缓存区数据
void printBuffer(Trajectory &trajectory_)
{
    std::cout << "Trajectory printBuffer  :  ";
    for (int i = 0; i < trajectory_.bufferSize; i++)
    {
        if(i == 0)
        {
            std::cout << "STATE : " << (char)trajectory_.buffer[i] << " ";
            continue;
        }
        if (i % 20 == 1)
        {
            std::cout << std::endl;
        }
        std::cout << hex << (int)trajectory_.buffer[i] << " ";
    }
    std::cout << dec << std::endl;
}


void printBuffer(ComplexPoint &point_)
{
    std::cout << "printBuffer  :  ";
    for (int i = 0; i < point_.bufferSize; i++)
    {
        if(i == 0)
        {
            std::cout << "STATE : " << (char)point_.buffer[i] << " ";
            continue;
        }
        if (i % 20 == 1)
        {
            std::cout << std::endl;
        }
        std::cout << hex << (int)point_.buffer[i] << " ";
    }
    std::cout << dec << std::endl;
}