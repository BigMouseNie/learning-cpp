#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>


template <typename T>
class BlockingQueuePro
{

public:
    BlockingQueuePro(bool noblock = false)
        : noblock_(noblock)
    {}

    void Push(const T& value)
    {
        std::lock_guard<std::mutex> producer_lock(producer_mutex_);
        producer_queue_.push(value);
        no_empty_.notify_one();
    }

    bool Pop(T& value)
    {
        std::lock_guard<std::mutex> worker_lock(worker_mutex_);
        if (worker_queue_.empty())
        {
            std::unique_lock<std::mutex> producer_lock(producer_mutex_);
            no_empty_.wait(producer_lock, [this](){return !producer_queue_.empty() || noblock_;});
            if (noblock_)
                return false;
            worker_queue_.swap(producer_queue_);   
        }
        value = worker_queue_.front(); 
        worker_queue_.pop();
        return true;
    }

    bool _Pop(T& value)
    {
        std::lock_guard<std::mutex> worker_lock(worker_mutex_);
        if (worker_queue_.empty() && SwapQueue() == 0)
        {
            return false;
        }

        value = worker_queue_.front(); 
        worker_queue_.pop();
        return true;
    }

    void Cancel()
    {
        std::lock_guard<std::mutex> producer_lock(producer_mutex_);
        std::lock_guard<std::mutex> worker_lock(worker_mutex_);
        noblock_ = true;
        no_empty_.notify_all();
    }

private:

    int SwapQueue()
    {
        std::unique_lock<std::mutex> producer_lock(producer_mutex_);
        no_empty_.wait(producer_lock, [this](){return !producer_queue_.empty || noblock;});
        std::swap(worker_queue_, producer_queue_);
        return worker_queue_.size();
    }


    std::queue<T> producer_queue_;
    std::queue<T> worker_queue_;
    std::mutex producer_mutex_;
    std::mutex worker_mutex_;
    bool noblock_;
    std::condition_variable no_empty_;
};

template <typename T>
class BlockingQueue
{
public:

    BlockingQueue(bool noblock = false)
        : noblock_(noblock)
    {}

    void Push(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
        no_empty_.notify_one();
    }

    bool Pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        no_empty_.wait(lock, [this] {return !queue_.empty() || noblock_;});

        if (queue_.empty())
            return false;

        value = queue_.front();
        queue_.pop();
        return true;
    }

    void Cancel()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        noblock_ = true;
        no_empty_.notify_all();
    }

private:
    bool noblock_;
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable no_empty_;
};


class ThreadPool
{
public:
    explicit ThreadPool(size_t num_thread)
    {
        for (size_t i = 0; i<num_thread ; ++i)
        {
            workers_.emplace_back([this]{Worker();});
        }
    }

    ~ThreadPool()
    {
        task_queue_.Cancel();
        for (auto& worker : workers_)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    template <typename F, typename... Args>
    void Post(F&& f, Args&&... args)
    {
        auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        task_queue_.Push(task);
    }

private:

    void Worker()
    {
        while (true)
        {
            std::function<void()> task;
            if (!task_queue_.Pop(task))
                break;
            task();
        }
    }

    BlockingQueuePro<std::function<void()>> task_queue_;
    std::vector<std::thread> workers_;
};

