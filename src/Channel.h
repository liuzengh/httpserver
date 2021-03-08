#pragma once
#include <poll.h>
#include <functional>
#include <memory>
#include "HttpData.h"
class EventLoop;
class HttpData;
//待学习： std::enable_shared_from_this <Channel>
class Channel: public std::enable_shared_from_this <Channel>
{
    public:
        Channel(EventLoop *loop);
        Channel(EventLoop *loop, int fd);
        
        ~Channel() = default;
        void handleEvents();
        void setEvent(__uint32_t event) { event_ = event; };
        void setRevent(__uint32_t revent) { revent_ = revent; };
        __uint32_t& getEvent() { return event_; }; 
        int getFd() const { return fd_; }
        void setFd(int fd) { fd_ = fd; };

        __uint32_t getWhatEvent() const { return whatEvent_; }
        void setWhatEvent(__uint32_t whatEvent)  { whatEvent_ = whatEvent; }


        void handleRead();
        void handleWrite();
        void handleConn();
        void handleError();
        void setReadHandler(std::function<void()> &&readHandler) { readHandler_ = readHandler; }
        void setWriteHandler(std::function<void()> &&writeHandler) { writeHandler_ = writeHandler; }
        void setErrorHandler(std::function<void()> &&errorHandler) { errorHandler_ = errorHandler; }
        void setConnHandler(std::function<void()> &&connHandler)  { connHandler_ = connHandler; }
        


        EventLoop* ownerLoop() { return loop_; };

        void setHolder( std::shared_ptr<HttpData> holder) { holder_ = holder; }
        std::shared_ptr<HttpData> getHolder() { return holder_.lock(); }
        

    public:
        static const __uint32_t kNoneEvent;
        static const __uint32_t kReadEvent;
        static const __uint32_t kWriteEvent;


    private:
        int fd_;               /* File descriptor to poll.  */
        __uint32_t event_;     /* Types of events poller cares about.  */
        __uint32_t revent_;    /* Types of events that actually occurred.  */
        __uint32_t whatEvent_;   /* kNoneEvent, kReadEvent,  kWriteEvent */

        EventLoop* loop_;
        
        /*CallBack function object*/
        std::function<void()> readHandler_;
        std::function<void()> writeHandler_;
        std::function<void()> errorHandler_;
        std::function<void()> connHandler_;
        std::weak_ptr<HttpData> holder_;


};