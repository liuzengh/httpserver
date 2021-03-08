#include "EventLoopThread.h"
#include <functional>
#include <cassert>

EventLoopThread::EventLoopThread(): 
    loop_(nullptr),
    exiting_(false)
    // EventLoopThread会启动自己的线程，并在其中运行EventLoop::loop()
{
  
}

EventLoopThread::~EventLoopThread() 
{
  exiting_ = true;
  if (loop_ != nullptr) 
  {
    loop_->quit();
    thread_.join();
  }
}
// 返回新线程中EventLoop对象的地址，因为EventLoop对象需要在线程函数创建之后才能被返回，所以需要用条件变量来等待线程的创建与运行
EventLoop* EventLoopThread::startLoop() {
  thread_ = std::thread(&EventLoopThread::threadFunc, this);
  {
    std::unique_lock<std::mutex> lk(mutex_);
    cond_.wait(lk, [this]{ return loop_ != nullptr; });
  }
  return loop_;
}

// 线程主函数在stack上定义EventLoop对象，然后将其地址赋值给loop_成员变量，最后notify()条件变量，唤醒startLoop()
void EventLoopThread::threadFunc() {
  EventLoop loop;
  {
    std::lock_guard<std::mutex> lk(mutex_);
    loop_ = &loop;
    cond_.notify_one();
  }
  loop.loop();
  loop_ = nullptr;
}