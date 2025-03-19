#include <atomic>

template<typename T>
class myshare_ptr
{
public:

myshare_ptr() :
    ptr(nullptr), ref_count(nullptr)
{}

explicit myshare_ptr(T* p) :
    ptr(p), ref_count(p ? new std::atomic<std::size_t>(1) : nullptr)
{}

~myshare_ptr()
{
    release();
}

/*
* 拷贝构造不能加explicit
* B b1();
* B b2 = b1; // 如果加上会报错(如果你没有重载“=”运算符)
* void func(B b){};
* func(b1); // 如果加上会报错，值传递会调用拷贝构造
*/
myshare_ptr(const myshare_ptr<T>& other) :
    ptr(other.ptr), ref_count(other.ref_count)
{
    if (ref_count)
    {
        ref_count->fetch_add(1);
    }
}

myshare_ptr& operator=(const myshare_ptr<T>& other)
{
    if (this != &other)
    {
        release();
        ref_count = other.get_ref_count();
        ptr = other.get_ptr();
        if (ref_count)
        {
            ref_count->fetch_add(1);
        }
    }
    return *this;
}

/*
* 移动构造看情况加explicit
* B func(){}
* B b1 = func(); // 如果加上会调用拷贝构造，反常识，别加
* B b2(func()); // 加不加都不会报错
*/
myshare_ptr(myshare_ptr<T>&& other) noexcept
    :ptr(other.ptr), ref_count(other.ref_count) 
{
    other.ptr = nullptr;
    other.ref_count = nullptr;
}

myshare_ptr& operator=(myshare_ptr<T>&& other) noexcept
{
    if (this != &other)
    {
        release();
        ptr = other.ptr;
        ref_count = other.ref_count;
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }
    return *this;
}

T* operator->() const
{
    return ptr;
}

T& operator*() const
{
    return *ptr;
}

T* get_ptr() const
{
    return ptr;
}

std::size_t get_ref_count() const
{
    return ref_count ? ref_count->load() : 0;
}

void reset(T* p = nullptr)
{
    release();
    ptr = p;
    ref_count = p ? new std::atomic<std::size_t>(1) : nullptr;
}

private:
T* ptr;
std::atomic<std::size_t>* ref_count;

void release()
{
    if (ref_count && ref_count->fetch_sub(1) == 1)
    {
        delete ref_count;
        delete ptr;   
    }
    ptr = nullptr;
    ref_count = nullptr;
}

};
