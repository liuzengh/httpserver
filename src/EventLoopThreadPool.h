#pragma once
#include "EventLoop.h"
#include "EventLoopThread.h"
class EventLoopThreadPool{
    public:
        EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
        // ~EventLoopThreadPool() = default;
        void start();
        EventLoop* getNextLoop();
        

    private:
        EventLoop *baseLoop_;
        int numThreads_;
        bool started_;
        int next_;
        
        std::vector<std::shared_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*> loops_;


};