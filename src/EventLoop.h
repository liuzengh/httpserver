#include "base/Thread.h"
#include "base/CurrentThread.h"
#include <memory>
#include <vector>

class Epoller;
class Channel;
class EventLoop: NotCopyable
{
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }
        void runInLoop(std::function<void()> &&cb);
        void queueInLoop(std::function<void()> &&cb);

        void quit();
        void updateChannel(std::shared_ptr<Channel> channel, int timeout = 0);
        void removeChannel(std::shared_ptr<Channel> channel);

    private:
        bool looping_;
        bool quitting_;
        const pid_t threadId_;
        std::unique_ptr<Epoller> poller_;

        mutable MutexLock mutex_;
        std::vector<std::function<void()>> pendingFunctors_;
        bool callingPendingFunctors_;

        int wakeupFd_;
        std::shared_ptr<Channel> pwakeupChannel_;
        

        void doPendingFunctors();
        void wakeup();
        void handleRead();
        void handleConn();

};