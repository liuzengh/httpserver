#include "EventLoop.h"
#include "EventLoopThread.h"
class EventLoopThreadPool{
    public:
        EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
        ~EventLoopThreadPool();
        void start();
        EventLoop* getNextLoop();
        

    private:
        EventLoop *baseLoop_;
        int numThreads_;
        
        int next_;
        bool started_;
        std::vector<std::shared_ptr<EventLoopThread>> threads_;
        std::vector<EventLoop*> loops_;


};