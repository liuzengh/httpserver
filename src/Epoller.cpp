#include "Epoller.h"
#include <cassert>
#include "Channel.h"
#include "log/logger.h"

Epoller::Epoller():
    kMAX_EVENT_NUMBER(4096), 
    kEVENT_TIMEOUT(10000),
    epollFd_(epoll_create1(EPOLL_CLOEXEC)), 
    events_(kMAX_EVENT_NUMBER)
{
    assert(epollFd_ > 0);
}


std::vector<std::shared_ptr<Channel>>  Epoller::poll()
{
    std::vector<std::shared_ptr<Channel>> activatedChannels;
    int activatedEventNumbers = epoll_wait(epollFd_, events_.data(), events_.size(), kEVENT_TIMEOUT); 
    if(activatedEventNumbers > 0)
    {
        for(int i = 0; i < activatedEventNumbers; ++i){
            int currentFd = events_[i].data.fd;
            std::shared_ptr<Channel> currentChannel = fd2channel_[currentFd];
            if(currentChannel)
            {
                __uint32_t whatEvent = Channel::kNoneEvent;
                if(events_[i].events & EPOLLIN) whatEvent |= Channel::kReadEvent;
                if(events_[i].events & EPOLLOUT) whatEvent |= Channel::kWriteEvent;
                // 仿照redis网络库的写法：把EPOLLERR和EPOLLHUP都转换成kWriteEvent和kReadEvent事件，显得很简洁
                if(events_[i].events & EPOLLERR) whatEvent |= Channel::kWriteEvent | Channel::kReadEvent;
                if(events_[i].events & EPOLLHUP) whatEvent |= Channel::kWriteEvent | Channel::kReadEvent;
                currentChannel->setWhatEvent(whatEvent);
                currentChannel->setRevent(events_[i].events);
                currentChannel->setEvent(0); 
                activatedChannels.emplace_back(currentChannel);
            }
            else
                LOG_ERROR("currentChannel is invalid"); 
        }
    }
    else if(activatedEventNumbers == 0)
       ;// LOG_INFO("poll nothing happened");
    else
        LOG_ERROR("poll");
    return activatedChannels;
}
void Epoller::updateChannel(std::shared_ptr<Channel> channel, int timeout)
{ 
    int fd = channel->getFd();
    if(timeout > 0)
        addTimer(channel, timeout);
    struct epoll_event event = {0};
    event.data.fd = fd;
    event.events = channel->getEvent();
    if(event.events & Channel::kReadEvent) event.events |= EPOLLIN;
    if(event.events & Channel::kWriteEvent) event.events |= EPOLLOUT;

    

    /* If the fd was already monitored for some event, we need a MOD
     * operation. Otherwise we need an ADD operation. */
    if(fd2channel_[fd]) 
    {
     //   LOG_INFO("mod fd:{}", fd);
        
        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) 
        {
            LOG_ERROR("updateChannel EPOLL_CTL_MOD method errno{}:{}", errno, strerror(errno));
            fd2channel_[fd].reset();
        }
    }
    else
    {
  //      LOG_INFO("add fd:{}", fd);
        fd2channel_[fd] = channel;
        fd2http_[fd] = channel->getHolder();
        if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) 
        {
            LOG_ERROR("updateChannel EPOLL_CTL_ADD method errno{}:{}", errno, strerror(errno));
            fd2channel_[fd].reset();
        }
    }

}

void Epoller::removeChannel(std::shared_ptr<Channel> channel)
{
    int fd = channel->getFd();
    struct epoll_event event = {0};
    /* In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required a non-null pointer in event,  even though this  argument  is ignored.
     * Since  Linux 2.6.9, event can be specified as NULL when using EPOLL_CTL_DEL */
    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0)
        LOG_ERROR("Remove channel EPOLL_CTL_DEL");
    fd2channel_[fd].reset();
    fd2http_[fd].reset(); // close fd
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
        LOG_ERROR("Add timer fail");
 }



