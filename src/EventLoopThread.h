#pragma once
#include "EventLoop.h"
#include "base/Condition.h"
#include "base/MutexLock.h"
#include "base/Thread.h"
#include "base/NotCopyable.h"


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
        Thread thread_;
        MutexLock mutex_;
        Condition cond_;
};