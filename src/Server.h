#pragma once
#include <memory>
#include <cstring>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server
{
    public:
        Server(EventLoop *loop, int threadNum, int port);
         // ~Server() = default;
        void start();
        void handleNewSocket();
       
    private:
        static const int kMaxFds;
        EventLoop *loop_;
        int numThreads_;
        int port_;
        bool started_;
        int listenFd_;
        std::shared_ptr<Channel> listenChannel_;

        std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;

         

};