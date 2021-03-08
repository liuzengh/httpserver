#pragma once
#include <queue>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include "../HttpData.h"

typedef std::chrono::steady_clock TimeClock;
typedef std::chrono::steady_clock::time_point TimeStamp;
class HttpData;
class TimerNode
{
    // 计时精确到毫秒
    public: 
        TimerNode(std::shared_ptr<HttpData> spHttpData, int timeout);
        TimerNode(const TimerNode &tn);
        ~TimerNode();
        void update(int timeout);
        bool isValid();
        void clearReq();
        void setDeleted() { deleted_ = true; }
        bool isDeleted() const { return deleted_; }
        TimeStamp getExpiredTime() const { return expiredTime_; };
    private:
        std::shared_ptr<HttpData> spHttpData_;
        TimeStamp expiredTime_;
        bool deleted_;

};
class TimerManager
{
    public:
        TimerManager() = default;
        ~TimerManager() = default;
        void handleExpriedEvent();
        void addTimer(std::shared_ptr<HttpData> spHttpData, int timeout);
    private:
        class TimerCmp 
        {
            public:
                bool operator()(const std::shared_ptr<TimerNode> &timer1, const std::shared_ptr<TimerNode> &timer2) const 
                {
                    return timer1->getExpiredTime() > timer2->getExpiredTime();
                }
        };
        std::priority_queue<std::shared_ptr<TimerNode>,  std::vector<std::shared_ptr<TimerNode>>,  TimerCmp> timerNodeQueue_;
        
};