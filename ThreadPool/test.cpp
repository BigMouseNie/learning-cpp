#include <iostream>
#include <chrono>
#include <atomic>

#include "threadpool.h"



std::atomic<size_t> task_counter{0};

void Task(size_t id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "Task_Id:" << id << ", exec by thread:" << std::this_thread::get_id() << std::endl;
    task_counter.fetch_add(1);
}

void Producer(ThreadPool& tp, size_t producer_id, size_t task_num)
{
    for (size_t i = 0; i < task_num; ++i)
    {
        size_t task_id = producer_id * 1000 + i;
        tp.Post(Task, task_id);
        std::cout << "Producer:" << producer_id << ", task:" << task_id << std::endl;
    }
}

int main()
{
    const size_t producer_num = 4;
    const size_t worker_num = 2;
    const size_t producer_task_num = 10;

    ThreadPool tp(worker_num);

    std::vector<std::thread> producers;

    for(size_t i = 1; i <= producer_num; ++i)
    {
        producers.emplace_back(Producer, std::ref(tp), i, producer_task_num);
    }

    for (auto& producer : producers)
    {
        producer.join();
    }

    while (task_counter < producer_num * producer_task_num)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}



