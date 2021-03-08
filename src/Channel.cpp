#include "Channel.h"
#include "EventLoop.h"
#include "log/logger.h"
#include <sys/epoll.h>
#include <sys/types.h>

const __uint32_t Channel::kNoneEvent = 0;
const __uint32_t Channel::kReadEvent = 1;
const __uint32_t Channel::kWriteEvent = 2;


Channel::Channel(EventLoop* loop):
    fd_(0), 
    event_(0), 
    whatEvent_(Channel::kNoneEvent), 
    loop_(loop)
    {

    }

Channel::Channel(EventLoop* loop, int fd):
    fd_(fd), 
    event_(0),  
    whatEvent_(Channel::kNoneEvent),
    loop_(loop)
    {
        
    }

void Channel::handleRead()
{
    if(readHandler_)    readHandler_();
};
void Channel::handleWrite()
{
    if(writeHandler_) writeHandler_();
};
void Channel::handleConn()
{
    if(connHandler_) connHandler_();
};
void Channel::handleEvents()
{
    event_ = Channel::kNoneEvent; 
    if(whatEvent_ & kReadEvent) 
        handleRead();
    else if(whatEvent_ & kWriteEvent) 
        handleWrite(); 
    // 处理ET模式。把该操作抽象成了handleConn()操作
    handleConn(); 
}