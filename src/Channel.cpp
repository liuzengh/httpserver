#include "Channel.h"
#include "EventLoop.h"
#include <sys/epoll.h>
#include <sys/types.h>

const __uint32_t Channel::kNoneEvent = 0;
const __uint32_t Channel::kReadEvent = POLLIN | POLLPRI;
const __uint32_t Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop):fd_(0), event_(0), levent_(0), loop_(loop)
{

}

Channel::Channel(EventLoop* loop, int fd):fd_(fd), event_(0), levent_(0), loop_(loop)
{

}


 void Channel::update()
{ 
     loop_->updateChannel(shared_from_this());
} 

void Channel::handleRead()
{
    if(readHandler_)    readHandler_();
};
void Channel::handleWrite()
{
    if(writeHandler_) writeHandler_();
};
void Channel::handleConn(){
    if(connHandler_) connHandler_();
};
void Channel::handleEvents()
{
    event_ = 0;
    if( (revent_ & EPOLLHUP && !(revent_ & EPOLLIN)))   return ;
    if( revent_ & EPOLLERR) 
    {
        if (errorHandler_) errorHandler_();
        return ;
    }

    if(revent_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) )    handleRead();
    if(revent_ & EPOLLOUT)  handleWrite();

    handleConn();
}