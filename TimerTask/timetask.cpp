
#include <utility>
#include <chrono>
#include <map>
#include <memory>
#include <sys/epoll.h>
#include <iostream>
#include <functional>
#include <unistd.h>

using namespace std;

class Timer;
class TimerTask {
    friend class Timer;
public:
    using CallBack = std::function<void(TimerTask* task)>;
    TimerTask(uint64_t addtime, uint64_t exectime, CallBack func)
    {
        m_addtime =  addtime;
        m_exectime = exectime;
        m_func = std::move(func);
    }

    uint64_t get_addtime() const
    {
        return m_addtime;
    }

    uint64_t get_exectime() const
    {
        return m_exectime;
    }

private:
    void run()
    {
        m_func(this);
    }

    uint64_t m_addtime;
    uint64_t m_exectime;
    CallBack m_func;
};

class Timer {
public:
    static uint64_t getTick() {
        return chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now().time_since_epoch()).count();
    }

    TimerTask* addTimeout(uint64_t offest, TimerTask::CallBack func)
    {
        auto now = getTick();
        auto exectime = now + offest;
        auto task = new TimerTask(now, exectime, std::move(func));
        auto ele = m_timeouts.emplace(exectime, task);
        return ele->second;
    }

    void delTimeout(TimerTask* task)
    {
        auto range = m_timeouts.equal_range(task->get_exectime());
        for (auto it = range.first; it != range.second;)
        {
            if (it->second == task)
            {
                it = m_timeouts.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

    void updateTimeout(uint64_t now)
    {
        auto it = m_timeouts.begin();
        while (it != m_timeouts.end() && it->first <= now)
        {
            it->second->run();
            delete it->second;
            it = m_timeouts.erase(it);
        }
    }

    int waitTime()
    {
        cout << "waitTime" << endl;
        auto it = m_timeouts.begin();
        if (it == m_timeouts.end())
        {
            return -1;
        }
        int diss = it->first - getTick();
        return diss > 0 ? diss : 0;
    }     

private:
    multimap<uint64_t, TimerTask*> m_timeouts;
};



int main()
{
    int epfd = epoll_create(1);
    unique_ptr<Timer> timer = make_unique<Timer>();

    int i = 1;
    timer->addTimeout(1000, [&](TimerTask* task){
        cout << Timer::getTick() << " addtime:" << task->get_addtime() << " revoked times:" << ++i << endl;
    });

    timer->addTimeout(2000, [&](TimerTask* task){
        cout << Timer::getTick() << " addtime:" << task->get_addtime() << " revoked times:" << ++i << endl;
    });

    timer->addTimeout(3000, [&](TimerTask* task){
        cout << Timer::getTick() << " addtime:" << task->get_addtime() << " revoked times:" << ++i << endl;
    });

    timer->addTimeout(2100, [&](TimerTask* task){
        cout << Timer::getTick() << " addtime:" << task->get_addtime() << " revoked times:" << ++i << endl;
    });

    cout << "now time:" << Timer::getTick() << endl;

    epoll_event ev[64] = {0};

    while (true)
    {
        cout << "wait time:" << timer->waitTime() << endl;
        // int n = epoll_wait(epfd, ev, 64, timer->waitTime());
        // time_t now = Timer::getTick();
        int waitTime = timer->waitTime();
        if (waitTime > 0) {
            usleep(waitTime * 1000);  // 将毫秒转换为微秒
        }
        time_t now = Timer::getTick();
        // for (int i=0 ; i<n; ++i)
        // {

        // }
        timer->updateTimeout(now);
    }
}
