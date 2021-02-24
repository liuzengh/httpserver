// g++ -pthread base/CountDownLatch.cpp base/Thread.cpp Channel.cpp Epoller.cpp EventLoop.cpp test01.cpp

#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>
#include <cstring>

EventLoop *g_loop;
void timeout()
{
    printf("Timeout!\n");
    g_loop->quit();
}
int main()
{
    EventLoop loop;
    g_loop = &loop;
    
    int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    
    std::shared_ptr<Channel> sp(new Channel(&loop, timerfd));
    sp->enableRead();
    sp->setReadHandler(timeout);
    

    struct itimerspec howlong;
    bzero(&howlong, sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timerfd, 0, &howlong, nullptr);
    loop.loop();

    return 0;
}