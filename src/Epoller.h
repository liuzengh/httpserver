#pragma once
#include <vector>
#include <memory>
#include <array>
#include "sys/epoll.h"
#include "NotCopyable.h"
#include "timer/Timer.h"


class Channel;

class Epoller: NotCopyable
{
    public:
        Epoller();
        //~Epoller() = default;
        std::vector< std::shared_ptr<Channel>> poll();

        /* Changes the interested I/O events, Must be called in the loop thread. */
        void updateChannel(std::shared_ptr<Channel> channel, int timeout = 0);
        void removeChannel(std::shared_ptr<Channel> channel);

        void handleExpiredEvent();
        void addTimer(std::shared_ptr<Channel> channel, int timeout);
    private:
        
        static const int kMAX_FDS = 100000;
        const int kMAX_EVENT_NUMBER;
        const int kEVENT_TIMEOUT;
        int epollFd_;

        TimerManager timerManager_;
        
        std::vector<struct epoll_event> events_;
        std::array<std::shared_ptr<Channel>, kMAX_FDS> fd2channel_;
        std::array<std::shared_ptr<HttpData>, kMAX_FDS> fd2http_;

};