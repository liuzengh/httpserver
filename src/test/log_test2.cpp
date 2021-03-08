/**
 * 测试Logger和BlockingQueue的基本功能
 * Logger和BlockingQueue是最简单的实现版本
 * 1. 生产者：在main线程中创建的4的子线程，消费者：后台异步线程
 * **/

#include "logger.h"
#include <chrono>
#include <thread>
#include <vector>

#include <unistd.h>
#include <sys/syscall.h>
static void threadFun(){
    auto id = std::this_thread::get_id();
    fmt::print("thread id : {}\n", (*(uint32_t*)&id));
    for(int i = 0; i < 10; ++i){
        logfun("Thread {}: step {}, log info - file{}:{}. \n", 
         syscall(SYS_gettid), i, __FILE__, __LINE__);
    }
}
int main()
{
    std::vector<std::thread> threads(4);
    for(auto &thread: threads){
        thread = std::thread(threadFun);
        thread.detach();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // 3s
    /*
    {
        logfun("log info in file{}, at line {}. \n", __FILE__, __LINE__);
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // 3s
    }*/
    fmt::print("main funtion\n");
    return 0;
}