#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "common.h"

//空间点
struct Point
{
    int duration;              //运动的持续时间  单位us
    int * postions;                     //位置，脉冲
    //std::vector<int> postions;      //位置，脉冲
    //std::vector<int> electric;      //电流，mA ，可选项
};


//轨迹
struct Trajectory
{
    uint8_t state;             //状态值，执行或取消
    int axies;                 //关节数
    
    int pointSize;             //单点缓存所占长度

    int pointsMaxCount;        //点的最大数量
    
    int pointsCount;           //点的数量

    int bufferMaxSize;         //流缓存区最大长度
    int bufferSize;            //已使用的缓存区长度，用于通讯发送接收数据
    
    //std::vector<Point> points; //路径点集合，用于对接应用程序
    Point * points;
    uint8_t * buffer;           //流缓存区，用于对接socket
};

//原本Trajectory就可以完成数据的反馈，但为了程序的可读性，添加并使用ComplexPoint
//复合空间点
struct ComplexPoint
{
    uint8_t state;             //状态值，执行或取消
    int axies;                 //关节数

    int pointSize;             //单点缓存所占长度

    int bufferMaxSize;         //流缓存区最大长度
    int bufferSize;            //已使用的缓存区长度，用于通讯发送接收数据

    Point point;               //点
    uint8_t * buffer;          //流缓存区，用于对接socket
};




//状态值
enum STATE
{
    PENDING = (uint8_t)'?',   //挂起态
    RUNING = (uint8_t)',',    //执行态
    STOPPED = (uint8_t)'!'    //静止态
};

// 符号位  0xf0 : "+" , 0x0f :"-"

// 单点数据结构 point
// | duration | postion_0 | postion_1 |   ......  | postion_n |
// |    2     |     3     |     3     |   ......  |     3     |
//    无符号      符号 + 2     符号 + 2     ......     符号 + 2
// 2 + (1+2) x axies = 2 + 3 x Axies
// Axies = 6   2 + 3 x 6 = 20

// 轨迹存储结构
// |  state   |  point_0  |  point_1  |   ......  |  point_n  |
// |    1     |    20     |    20     |   ......  |    20     |
//    无符号
// 1 + Points x ( 2 + 3 x Axies )
// Axies = 6    1 + 20 x Points

//应用数据结构转为流结构
void trajectory2buffer(Trajectory &trajectory_);
void point2buffer(ComplexPoint & point_);

//流结构转为应用数据结构
void buffer2trajectory(Trajectory &trajectory_);
void buffer2point(ComplexPoint & point_);

//初始化缓存区
void initTrajectoryBuffer(Trajectory &trajectory_);
void initComplexPointBuffer(ComplexPoint & point_);         

//打印基本信息
void printInfo(Trajectory &trajectory_);
void printInfo(ComplexPoint &point_);

//打印路径
void printTrajectory(Trajectory &trajectory_);
void printComplexPoint(ComplexPoint &point_);

//打印缓存区
void printBuffer(Trajectory &trajectory_);
void printBuffer(ComplexPoint &point_);

#endif //PROTOCOL_H