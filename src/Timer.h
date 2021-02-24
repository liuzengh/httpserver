#include <queue>
#include <memory>
#include <vector>
#include <functional>
#include "HttpData.h"

class TimerNode
{
    public:
        TimerNode(std::shared_ptr<HttpData> spHttpData, int timeout);
        TimerNode(TimerNode &tn);
        ~TimerNode();
        void update(int timeout);
        bool isValid();
        void clearReq();
        void setDeleted() { deleted_ = true; }
        bool isDeleted() const { return deleted_; }
        size_t getExpiredTime() const { return expiredTime_; };
    private:
        
        
        std::shared_ptr<HttpData> spHttpData_;
        bool deleted_;
        size_t expiredTime_;

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