#include <poll.h>
#include <functional>
#include <memory>
#include "HttpData.h"
class EventLoop;
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
        void setLastEvent() { levent_ = event_; }
        __uint32_t getLastEvent() { return levent_; }
        int getFd() { return fd_; }
        void setFd(int fd) { fd_ = fd; };




        void handleRead();
        void handleWrite();
        void handleConn();
        void handleError();
        void setReadHandler(std::function<void()> &&readHandler) { readHandler_ = readHandler; }
        void setWriteHandler(std::function<void()> &&writeHandler) { writeHandler_ = writeHandler; }
        void setErrorHandler(std::function<void()> &&errorHandler) { errorHandler_ = errorHandler; }
        void setConnHandler(std::function<void()> &&connHandler)  { connHandler_ = connHandler; }
        void update(); 
        void enableRead(){  event_ |= kReadEvent; update(); };
        void enableWrite() { event_ |= kReadEvent; update(); };
        void enableAll()    {event_ |= (kReadEvent | kWriteEvent); update(); };
        void disableWrite() { event_ &= ~kWriteEvent; update(); }
        void disableAll() { event_ = kNoneEvent; update(); }

        EventLoop* ownerLoop() { return loop_; };

        void setHolder( std::shared_ptr<HttpData> holder) { holder_ = holder; }
        std::shared_ptr<HttpData> getHolder() { return holder_.lock(); }
        




    private:
        int fd_;               /* File descriptor to poll.  */
        __uint32_t event_;     /* Types of events poller cares about.  */
        __uint32_t revent_;    /* Types of events that actually occurred.  */
        __uint32_t levent_;     /* Types of events that last time cares about. */

        EventLoop* loop_;
        
        /*CallBack function object*/
        std::function<void()> readHandler_;
        std::function<void()> writeHandler_;
        std::function<void()> errorHandler_;
        std::function<void()> connHandler_;

        static const __uint32_t kNoneEvent;
        static const __uint32_t kReadEvent;
        static const __uint32_t kWriteEvent;
        

        std::weak_ptr<HttpData> holder_;
    

};