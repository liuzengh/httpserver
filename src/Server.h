#include <memory>
#include "Channel.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

class Server
{
    public:
        Server(EventLoop *loop, int threadNum, int port);
         ~Server();
        void start();
        void handleConnctSocket();
        void handleListenSocket();
       
    private:
        static const int kMaxFds;
        EventLoop *loop_;
        int numThreads_;
        int port_;

        int listenFd_;
        std::shared_ptr<Channel> listenChannel_;

        std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_;

         bool started_;

};