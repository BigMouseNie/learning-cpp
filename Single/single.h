



/**
 * 不可以在析构函数中delet instance，会引发无限递归
 */
class Single_1
{
public:
static Single_1* getInstance()
{
    if (nullptr == instance)
    {
        instance = new Single_1();
    }
    return instance;
}

private:
static Single_1* instance;

Single_1(){}
~Single_1(){}
Single_1& Single_1(const Single_1&) = delete;
Single_1& operator=(const Single_1&) = delete;
Single_1& Single_1(Single_1&&) = delete;
Single_1& operator=(Single_1&&  ) = delete;

};

Single_1* Single_1::instance = nullptr;


class Single_2
{
public:
static Single_2* getInstance()
{
    if (nullptr == instance)
    {
        instance = new Single_2();
        atexit(Destructor);
    }
    return instance;
}

private:
static Single_2* instance;

static void Destructor()
{
    if (nullptr != instance)
    {
        delete instance;
        instance = nullptr;
    }
}

Single_2(){}
~Single_2(){}
Single_2& Single_2(const Single_2&) = delete;
Single_2& operator=(const Single_2&) = delete;
Single_2& Single_2(Single_2&&) = delete;
Single_2& operator=(Single_2&&  ) = delete;
Single_2
};

Single_2* Single_2::instance = nullptr;




class Single_3
{
public:

/**
 * 线程安全，但是颗粒度较大
 */
// static Single_3* getInstance()
// {
//     lock_guard<std::mutex> lock(m_mutex);
//     if (nullptr == instance)
//     {
//         instance = new Single_3();
//         atexit(Destructor);
//     }
//     return instance;
// }


/**
 * 双检测，颗粒度小
 */
static Single_3* getInstance()
{
    
    if (nullptr == instance)
    {
        lock_guard<std::mutex> lock(m_mutex);
        if (nullptr == instance) // 避免多线程同时请求lock导致new多次
        {
            instance = new Single_3();
            atexit(Destructor);
        }
    }
    return instance;
}

private:
static Single_3* instance;
static std::mutex m_mutex;

static void Destructor()
{
    if (nullptr != instance)
    {
        delete instance;
        instance = nullptr;
    }
}

Single_3(){}
~Single_3(){}
Single_3& Single_3(const Single_3&) = delete;
Single_3& operator=(const Single_3&) = delete;
Single_3& Single_3(Single_3&&) = delete;
Single_3& operator=(Single_3&&  ) = delete;
};

Single_3* Single_3::instance = nullptr;
std::mutex Single_3::m_mutex;


/**
 * 不明确
 */

class Single_4
{
public:
static Single_4* getInstance()
{
    Single_4* tmp = instance.load(std::memory_order_relaxed);
    std::atomic_thread_fence(std::memory_order_acquire);
    if (nullptr == tmp)
    {
        lock_guard<std::mutex> lock(m_mutex);
        Single_4* tmp = instance.load(std::memory_order_relaxed);
        if (nullptr == instance)
        {
            tmp = new Single_4();
            std::atomic_thread_fence(std::memory_order_release);
            instance.store(tmp, std::memory_order_relaxed);
            atexit(Destructor);
        }
    }
    return tmp;
}

private:
static std::atomic<Single_4*> instance;
static std::mutex m_mutex;

static void Destructor()
{
    Single_4* tmp = instance.load(std::memory_order_relaxed);
    if (nullptr != tmp)
    {
        delete tmp;
    }
}

Single_4(){}
~Single_4(){}
Single_4& Single_4(const Single_4&) = delete;
Single_4& operator=(const Single_4&) = delete;
Single_4& Single_4(Single_4&&) = delete;
Single_4& operator=(Single_4&&  ) = delete;
};

std::atomic<Single_4*> Single_4::instance;
std::mutex Single_4::m_mutex;



/**
 * 最简单
 */
class Single_5
{
public:
static Single_5* getInstance()
{
    static Single_5 instance;
    return &instance;
}

private:

Single_5(){}
~Single_5(){}
Single_5& Single_5(const Single_5&) = delete;
Single_5& operator=(const Single_5&) = delete;
Single_5& Single_5(Single_5&&) = delete;
Single_5& operator=(Single_5&&  ) = delete;
};


template <typename T>
class Single
{
public:
    static T* getInstance()
    {
        static T instance;
        return &instance;
    }
protected:
    Single(){}
    virtual ~Single(){}

private:
Single& Single(const Single&) = delete;
Single& operator=(const Single&) = delete;
Single& Single(Single&&) = delete;
Single& operator=(Single&&  ) = delete;
};


class MyClass : public Single<MyClass>
{
    friend class Single<MyClass>; // Single::getInstance需要访问MyClass的构造函数(私有)

public:
private:
    MyClass(){}
    ~MyClass(){}
};
