
//#include "base/Thread.h"
//#include "base/CurrentThread.h"
#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <functional>
#include "NotCopyable.h"


class Epoller;
class Channel;
class EventLoop: NotCopyable
{
    public:
        EventLoop();
        ~EventLoop();
        void loop();
        bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }
        void runInLoop(std::function<void()> &&cb);
        void queueInLoop(std::function<void()> &&cb);

        void quit();
        void updateChannel(std::shared_ptr<Channel> channel, int timeout = 0);
        void removeChannel(std::shared_ptr<Channel> channel);

    private:
        bool looping_;
        bool quitting_;
        const std::thread::id threadId_;
        std::unique_ptr<Epoller> poller_;

        
        mutable std::mutex mutex_;
        std::vector<std::function<void()>> pendingFunctors_;
        bool callingPendingFunctors_;

        int wakeupFd_;
        std::shared_ptr<Channel> pwakeupChannel_;
        

        void doPendingFunctors();
        void wakeup();
        void handleRead();

};