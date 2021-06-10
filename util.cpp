#include "util.h"

void Log(std::string str)
{
    std::cout << "[LOG]  " << str << std::endl;
}

void Log(std::string str1, double str2)
{
    std::cout << "[LOG]  " << str1 << " : " << str2 << std::endl;
}

void Log(std::string str1, std::string str2)
{
    std::cout << "[LOG]  " << str1 << " : " << str2 << std::endl;
}