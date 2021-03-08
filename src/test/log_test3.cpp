/** g++ log_test3.cpp format.cc -o test3 -lpthread
 * 测试Logger和BlockingQueue的基本功能
 * Logger和BlockingQueue是最简单的实现版本
 * 1. 生产者：在main线程中创建的4的子线程，消费者：后台异步线程
 * 2. 测试logger，日志级别和日志格式, 4百万无问题
 * 
 * **/

#include <linux/sched.h>

#include "logger.h"
#include <chrono>
#include <thread>
#include <vector>

#include <unistd.h>
#include <sys/syscall.h>


static void threadFun(){

    auto id = std::this_thread::get_id();
    
    for(int i = 0; i < 1000000; ++i)
    {
        LOG_INFO(" I'm very good {} ", i);
    }
}
int main()
{
    
    std::vector<std::thread> threads(4);
    for(auto &thread: threads){
        thread = std::thread(threadFun);
        thread.detach();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300000)); // 3s
    /*
    {
        logfun("log info in file{}, at line {}. \n", __FILE__, __LINE__);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // 3s
    }*/
    fmt::print("main funtion\n");
    return 0;
}