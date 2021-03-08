#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>

#include "EventLoop.h"
//#include "base/Condition.h"
//#include "base/MutexLock.h"
//#include "base/Thread.h"
//#include "base/NotCopyable.h"
#include "NotCopyable.h"

class EventLoopThread : NotCopyable 
{
    public:
        EventLoopThread();
        ~EventLoopThread();
        EventLoop* startLoop();
        
    private:
        void threadFunc();
        EventLoop* loop_;
        bool exiting_;

        std::thread thread_;
        std::mutex mutex_;
        std::condition_variable cond_;
};