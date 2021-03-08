#include "Timer.h"

TimerNode::TimerNode(std::shared_ptr<HttpData> spHttpData, int timeout):
    spHttpData_(spHttpData),
    deleted_(false)
    {
        expiredTime_ = TimeClock::now() + std::chrono::milliseconds(timeout);
    }

TimerNode::TimerNode(const TimerNode &tn): 
    spHttpData_(tn.spHttpData_), 
    expiredTime_(tn.getExpiredTime())  
    {

    }

TimerNode::~TimerNode()
{
    if(spHttpData_) spHttpData_->handleClose(); 
}

void TimerNode::update(int timeout) 
{
  expiredTime_ = TimeClock::now() + std::chrono::milliseconds(timeout);
}

bool TimerNode::isValid() {
    return !deleted_ && TimeClock::now() < expiredTime_;
}

void TimerNode::clearReq() {
    spHttpData_.reset();
    this->setDeleted();
}



void TimerManager::addTimer(std::shared_ptr<HttpData> spHttpData, int timeout)
{
    std::shared_ptr<TimerNode> spTimeNode(new TimerNode(spHttpData, timeout));
    timerNodeQueue_.push(spTimeNode);
    spHttpData->linkTimer(spTimeNode); 
}
void TimerManager::handleExpriedEvent()
{
    while(!timerNodeQueue_.empty() && !timerNodeQueue_.top()->isValid())
        timerNodeQueue_.pop();
}