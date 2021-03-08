#include "EventLoop.h"
#include "Epoller.h"
#include "Channel.h"
#include "Util.h"
#include "log/logger.h"

#include <iostream>
#include <vector>
#include <cassert>

#include <sys/eventfd.h>


__thread EventLoop* t_loopInThisThread = nullptr; // fixme: seem useless

int createEventfd()
{
    int eventfd_ =  eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(eventfd_ < 0)
    {
        perror("Failed in eventfd");
        abort();
    }
    return eventfd_;
}

EventLoop::EventLoop():
    looping_(false), 
    quitting_(false), 
    threadId_(std::this_thread::get_id()),
    poller_(new Epoller()),  // fix me
    callingPendingFunctors_(false),
    wakeupFd_(createEventfd()),
    pwakeupChannel_(new Channel(this, wakeupFd_))
{
    if(t_loopInThisThread)
        perror( "Another EventLoop  exists in this thread");
    else
        t_loopInThisThread = this;
   // LOG_INFO("wakeup channel created, fd is:{}", wakeupFd_);
    pwakeupChannel_->setEvent(EPOLLIN | EPOLLET);
    pwakeupChannel_->setWhatEvent(Channel::kReadEvent);
    pwakeupChannel_->setReadHandler(std::bind(&EventLoop::handleRead, this)); 
    poller_->updateChannel(pwakeupChannel_); 
}

EventLoop::~EventLoop()
{
    close(wakeupFd_);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop()
{
    assert(!looping_);
    assert(isInLoopThread());
    looping_ = true;

    while(!quitting_)
    {
        std::vector< std::shared_ptr<Channel> > activatedChannels = poller_->poll();
        for(auto &&activatedChannel: activatedChannels)
        {
            activatedChannel->handleEvents();
        }
        doPendingFunctors();
        poller_->handleExpiredEvent();
    }
    looping_ = false;
}

void EventLoop::runInLoop(std::function<void()> &&cb)
{
    if(isInLoopThread())
        cb();
    else
        queueInLoop(std::move(cb));    
}
void EventLoop::queueInLoop(std::function<void()> &&cb)
{
    {
        std::lock_guard<std::mutex> lk(mutex_);
        pendingFunctors_.emplace_back(cb);
    }
    if(!isInLoopThread() || callingPendingFunctors_)
    {
        // 1. 如果调用queueInLoop()的线程不是IO线程，那么唤醒是必需的
        // 2. 如果在IO线程调用queueInLoop()，而此时正在调用pending functor，那么也必须唤醒
        wakeup();
    }
}
void EventLoop::wakeup()
{
    __uint64_t oneWord = 1;
    ssize_t n = writen(wakeupFd_, (char*)(&oneWord), sizeof(oneWord));
    if(n != sizeof(oneWord))
    {
        printf("EventLoop::wakeup() writes %ld bytes instead of 8", n);
    }
}


void EventLoop::doPendingFunctors()
{
    std::vector<std::function<void()>> functors;
    callingPendingFunctors_ = true;
    {
        // 把回调列表swap()到局部变量functors中：
        // 1. 减小了临界区的长度（意味着不会阻塞其他线程调用queueInLoop()）
        // 2. 避免了死锁（因为Functor可能再调用queueInLoop()）
        std::lock_guard<std::mutex> lk(mutex_);
        functors.swap(pendingFunctors_); 
    }
    for(auto &functor: functors)    functor();
    callingPendingFunctors_ = false;
}

void EventLoop::quit()
{
    quitting_ = true;
    if(!isInLoopThread())
        wakeup();
}

void EventLoop::updateChannel(std::shared_ptr<Channel> channel, int timeout)
{
    assert(channel->ownerLoop() == this);
    assert(isInLoopThread());
    poller_->updateChannel(channel, timeout);
}
void EventLoop::removeChannel(std::shared_ptr<Channel> channel)
{
    poller_->removeChannel(channel);
}
void EventLoop::handleRead()
{
    __uint64_t oneWord = 1;
    ssize_t n = readn(wakeupFd_, &oneWord, sizeof(oneWord));
    if(n != sizeof(oneWord))
    {
        printf("EventLoop::handleRead() reads %ld bytes instead of 8\n", n);
    }
    pwakeupChannel_->setEvent(EPOLLIN | EPOLLET); // why? 因为是ET模式
    pwakeupChannel_->setWhatEvent(Channel::kReadEvent);
    poller_->updateChannel(pwakeupChannel_);

}

