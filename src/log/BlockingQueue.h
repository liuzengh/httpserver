#include <memory>
#include <mutex>
#include <condition_variable>
template<typename T>
// unbounded queue based on singly linked list( with dummy node) with fine-grained locking
class BlockingQueue
{
private:
    struct node
    { 
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node *tail;
    std::condition_variable data_cond;

    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    };
    std::unique_ptr<node> pop_head()
    {
        std::unique_ptr<node> old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }
    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get() == get_tail())
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }
    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock, [&]{return head.get() != get_tail(); });
        return pop_head();
    };
public:
    BlockingQueue():head(new node), tail(head.get()){};
    BlockingQueue(const BlockingQueue &other) = delete;
    BlockingQueue& operator=(const BlockingQueue& other) = delete;
    
    bool empty();
    void push(T new_value);
    std::shared_ptr<T> try_pop();
    std::shared_ptr<T> wait_and_pop();
};
template<typename T>
bool BlockingQueue<T>::empty()
{
    std::lock_guard<std::mutex> head_lock(head_mutex);
    return (head.get() == get_tail());
};

template<typename T>
void BlockingQueue<T>::push(T new_value)
{
    std::shared_ptr<T> new_data = std::make_shared<T>(std::move(new_value));
    std::unique_ptr<node> p(new node);
    {
        // std::unique_lock<std::mutex> tail_lock(tail_mutex);
        // data_cond.wait(tail_lock, [&]{ return size < capacity;}) 
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        node* const new_tail = p.get();
        tail->next = std::move(p);
        tail = new_tail;
    }
    data_cond.notify_one();
}
template<typename T> 
std::shared_ptr<T> BlockingQueue<T>::try_pop()
{
    std::unique_ptr<node> old_head = try_pop_head();
    return old_head ? old_head->data : std::shared_ptr<T>();
}

template<typename T>
std::shared_ptr<T>  BlockingQueue<T>::wait_and_pop()
{
    std::unique_ptr<node> const old_head = wait_pop_head();
    return old_head->data;
};