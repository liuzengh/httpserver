/**
 * 测试Logger和BlockingQueue的基本功能
 * Logger和BlockingQueue是最简单的实现版本
 * 1. 生产者：main线程，消费者：后台异步线程
 * **/

#include "logger.h"
#include <chrono>
#include <thread>

int main()
{
    {
        logfun("log info in file{}, at line {}. \n", __FILE__, __LINE__);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // 3s
    }
    fmt::print("main funtion\n");
    return 0;
}