#include "EventLoopThread.h"
#include <functional>
#include <cassert>

EventLoopThread::EventLoopThread(): 
    loop_(nullptr),
    exiting_(false),
    // EventLoopThread会启动自己的线程，并在其中运行EventLoop::loop()
    thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex_(),
    cond_(mutex_) 
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
// 返回新线程中EventLoop对象的地址，因为EventLoop对象需要在线程函数创建之后才能被返回，所以需要用条件变量来等待线程的创建与运行。
EventLoop* EventLoopThread::startLoop() {
  assert(!thread_.started());
  thread_.start();
  {
    MutexLockGuard lock(mutex_);
    while (loop_ == nullptr) cond_.wait();
  }
  return loop_;
}

// 线程主函数在stack上定义EventLoop对象，然后将其地址赋值给loop_成员变量，最后notify()条件变量，唤醒startLoop()。
void EventLoopThread::threadFunc() {
  EventLoop loop;
  {
    MutexLockGuard lock(mutex_);
    loop_ = &loop;
    cond_.notify();
  }
  loop.loop();
  loop_ = nullptr;
}