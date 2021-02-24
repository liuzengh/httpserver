#include "Epoller.h"
#include <cassert>
#include "Channel.h"

// 待学习：epoll_create1(int flags)的用法，和参数flag:EPOLL_CLOEXEC的含义。
Epoller::Epoller():
    kMAX_EVENT_NUMBER(4096), 
    kEVENT_TIMEOUT(10000),
    epollFd_(epoll_create1(EPOLL_CLOEXEC)), 
    events_(kMAX_EVENT_NUMBER)
{
    assert(epollFd_ > 0);
}

void Epoller::fillActivatedChannels(int activatedEventNumbers, std::vector< std::shared_ptr<Channel> >& activatedChannels)
{
    
    for(int i = 0; i < activatedEventNumbers; ++i){
        int currentFd = events_[i].data.fd;
        std::shared_ptr<Channel> currentChannel = fd2channel_[currentFd];
        if(currentChannel)
        {
            currentChannel->setRevent(events_[i].events);
            currentChannel->setEvent(0); 
            activatedChannels.emplace_back(currentChannel);
        }
        else
        {
            //  perror("currentChannel is invalid"); 
        }
    }
}

std::vector< std::shared_ptr<Channel> >  Epoller::poll()
{
    std::vector< std::shared_ptr<Channel> > activatedChannels;
    do
    {
        int activatedEventNumbers = epoll_wait(epollFd_, events_.data(), events_.size(), kEVENT_TIMEOUT); 
        if(activatedEventNumbers > 0)
        {
            fillActivatedChannels(activatedEventNumbers, activatedChannels);    
        }
        else if(activatedEventNumbers == 0)
        {
            // printf("nothing happened");
        }
        else
        {
            // perror("epoll wait error");
        }
        
    }while(activatedChannels.empty());

    return activatedChannels;
    
}
void Epoller::updateChannel(std::shared_ptr<Channel> channel, int timeout = 0)
{ 
    if(fd2channel_[channel->getFd()])
        epollMod(channel, timeout);
    else
        epollAdd(channel, timeout);
}

void Epoller::removeChannel(std::shared_ptr<Channel> channel)
{
    epollDel(channel);
}

void Epoller::handleExpiredEvent()
{
    timerManager_.handleExpriedEvent();
}
 void Epoller::addTimer(std::shared_ptr<Channel> channel, int timeout)
 {
    std::shared_ptr<HttpData> t = channel->getHolder();
    if(t)
        timerManager_.addTimer(t, timeout);
    else
        perror("timer add fail");
 }


void Epoller::epollAdd(std::shared_ptr<Channel> channel, int timeout = 0)
{
    
    if(timeout > 0)
    {
        addTimer(channel, timeout);
        // fixme: add fd to http;
    }
    int fd = channel->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = channel->getEvent();
    channel->setLastEvent(); 
    fd2channel_[fd] = channel;
    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) 
    {
        perror("epoll_add error");
        fd2channel_[fd].reset();
    }

};

void Epoller::epollMod(std::shared_ptr<Channel> channel, int timeout = 0)
{
    if(timeout > 0)
        addTimer(channel, timeout);
    int fd = channel->getFd();
    if (channel->getLastEvent() != channel->getEvent()) 
    {
        struct epoll_event event;
        event.data.fd = fd;
        event.events = channel->getEvent();
        channel->setLastEvent(); 
        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) 
        {
            perror("epoll_mod error");
            fd2channel_[fd].reset();
        }
    }
};

void Epoller::epollDel(std::shared_ptr<Channel> channel)
{
    int fd = channel->getFd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = channel->getEvent();

    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        perror("epoll_del error");
    }
    fd2channel_[fd].reset();
}