#ifndef COMMON_H
#define COMMON_H

//所以平时需要用到的头文件都放在这里
#include <iostream>
#include <stdio.h>
#include <inttypes.h>
#include <cstring>
#include <cmath>
#include <signal.h>
#include <vector>

//tcp
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

namespace MyFunctions
{
    extern bool condition;      //循环判断条件   false为不符合条件   true为符合条件
    extern void stop(int sign); //捕获信号量
    extern const bool ok();     //反馈condition变量
} // namespace MyFunctions

#endif // COMMON_H