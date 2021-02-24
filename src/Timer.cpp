#include "Timer.h"
#include <sys/time.h>
TimerNode::TimerNode(std::shared_ptr<HttpData> spHttpData, int timeout):
    spHttpData_(spHttpData),
    deleted_(false)
    
{
    struct timeval now;
    gettimeofday(&now, nullptr); // fixme：时间计算有问题
    expiredTime_ = timeout +  (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
}

TimerNode::TimerNode(TimerNode &tn): 
    expiredTime_(0), 
    spHttpData_(tn.spHttpData_) //fixme
{

}

TimerNode::~TimerNode()
{
    if(spHttpData_) spHttpData_->handleClose(); // fixme 
}

void TimerNode::update(int timeout) {
  struct timeval now;
  gettimeofday(&now, NULL);
  expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

bool TimerNode::isValid() {
    struct timeval now;
    gettimeofday(&now, NULL);
    size_t temp = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if (temp < expiredTime_)
        return true;
    else {
        this->setDeleted();
        return false;
    }
}

void TimerNode::clearReq() {
    spHttpData_.reset();
    this->setDeleted();
}






void TimerManager::addTimer(std::shared_ptr<HttpData> spHttpData, int timeout)
{
    std::shared_ptr<TimerNode> spTimeNode(new TimerNode(spHttpData, timeout));
    timerNodeQueue_.push(spTimeNode);
    spHttpData->linkTimer(spTimeNode); // fixme
}
void TimerManager::handleExpriedEvent()
{
    while(!timerNodeQueue_.empty())
    {
        std::shared_ptr<TimerNode>  spTimeNode = timerNodeQueue_.top();
        if((spTimeNode->isDeleted()) || !spTimeNode->isValid())
            timerNodeQueue_.pop();
        else   
            break;
    }
}