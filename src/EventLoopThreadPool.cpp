#include "EventLoopThreadPool.h"

#include <cassert>

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop, int numThreads):
    baseLoop_(loop),
    numThreads_(numThreads),
    started_(false),
    next_(0)
{

    if(numThreads_ < 0)
    {
        perror("numThreads_ <= 0");
        abort();
    }
}

void EventLoopThreadPool::start()
{
    assert(baseLoop_->isInLoopThread());
    started_ = true;
    for(int i = 0; i < numThreads_; ++i)
    {
        std::shared_ptr<EventLoopThread> thread(new EventLoopThread());
        threads_.emplace_back(thread);
        loops_.emplace_back(thread->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    assert(baseLoop_->isInLoopThread());
    assert(started_);
    EventLoop *loop = baseLoop_;
    if(!loops_.empty())
    {
        loop = loops_[next_];
        next_ = (next_ + 1) % numThreads_; 
    }
    return loop;
}