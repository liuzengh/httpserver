#include <chrono>
#include <iostream>
#include <functional>
int main()
{
    std::cout << "毫秒" << std::endl;
    return 0;
    std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
}

typedef std::chrono::steady_clock::time_point TimeStamp;
struct TimerNode
{
    TimeStamp expires_;
    std::function<void()> callBack_;
    bool operator<(const TimerNode& t) { return expires_ < t.expires_; }
};