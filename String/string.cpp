#include <iostream>
#include <cstring>

/**
 * 关于C-style字符串
 * 对于char[]和char*类型，strlen会获取字符串长度，但是不包括“\0”
 * 对于char[],sizeof会获取总长度，包括“\0”
 * 对于char*,sizeof会获取指针长度，64位系统为8
 * strcpy(char* dest, char* src);在dest大小足够的情况下，会复制到src数组的“\0”处
 * memcpy(char* dest, char* src, len);字节级拷贝，自己把握，肯定要考虑到“\0”
 * char* p = "hello"; // "hello" 为常量，无法更该(约等于 const char* p = "hello";)
 */







class MyString
{
public:
    static const size_t min_capacity;
    static const size_t exp_factor;

private:
    // m_size 和 m_capacity 都不包含“\0”
    char* m_data;
    size_t m_size;
    size_t m_capacity;

    void realloc_data (size_t new_cap)
    {
        new_cap = std::max(new_cap, min_capacity);
    
        char* cp = new char[new_cap + 1];
        if (m_size > 0)
            memcpy(cp, m_data, m_size);
        cp[m_size] = '\0';
        delete[] m_data;
        m_data = cp;
        m_capacity = new_cap;
    }

public:

    // 默认构造
    MyString()
        :m_size(0), m_capacity(min_capacity) 
    {
        m_data = new char[min_capacity + 1];
        m_data[0] = '\0';
    }

    ~MyString()
    {
        if (m_data)
        {
            delete[] m_data;
            m_data = nullptr;
        }
    }

    // C-style字符串构造
    MyString(const char * const cv)
    {
        if (nullptr == cv)
        {
            throw std::invalid_argument("char* is nullptr");
        }

        size_t len = std::strlen(cv);
        m_size = len;
        m_capacity = std::max(len, min_capacity);
        m_data = new char[m_capacity + 1];
        memcpy(m_data, cv, m_size);
        m_data[m_size] = '\0';
    }

    MyString(const void * const data, size_t size)
    {
        if (nullptr == data)
        {
            throw std::invalid_argument("char* is nullptr");
        }

        m_size = size;
        m_capacity = std::max(size, min_capacity);
        m_data = new char[m_capacity + 1];
        memcpy(m_data, data, size);
        m_data[m_size] = '\0';
    }

    // 拷贝构造
    MyString(const MyString& other)
        :m_size(other.m_size), m_capacity(other.m_capacity)
    {
        m_data = new char[m_capacity + 1];
        memcpy(m_data, other.m_data, m_size);
        m_data[m_size] = '\0';
    }

    // 重载"="
    MyString& operator=(const MyString& other)
    {
        if (this == &other)
        {
            return *this;
        }

        m_size = other.m_size;
        m_capacity = other.m_capacity;
        delete[] m_data;
        m_data = new char[m_capacity + 1];
        memcpy(m_data, other.m_data, m_size);
        m_data[m_size] = '\0';
        return *this;
    }

    MyString(MyString&& other)
        :m_size(other.m_size), m_capacity(other.m_capacity), m_data(other.m_data)
    {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    MyString& operator=(MyString&& other)
    {
        if (this == &other)
        {
            return *this;
        }

        m_size = other.m_size;
        m_capacity = other.m_capacity;
        m_data = other.m_data;
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
        return *this;
    }

    void append(const MyString& other)
    {
        size_t new_size = m_size + other.m_size;
        if (new_size > m_capacity)
        {
            realloc_data(new_size * exp_factor);
        }

        memcpy(m_data + m_size, other.m_data, other.m_size);
        m_data[new_size] = '\0';
        m_size = new_size;
    }

    void append(const char * const cv)
    {
        if (nullptr == cv)
        {
            throw std::invalid_argument("char* is nullptr");
        }

        size_t len = std::strlen(cv);
        size_t new_size = m_size + len;
        if (new_size > m_capacity)
        {
            realloc_data(new_size * exp_factor);
        }

        memcpy(m_data + m_size, cv, len);
        m_data[new_size] = '\0';
        m_size = new_size;
    }

    const char * const c_str() const noexcept {return m_data;}
};

const size_t MyString::min_capacity = 15;
const size_t MyString::exp_factor = 2;






int main()
{
    MyString str1;
    MyString str2("qwe");
    str1.append(str2);
    std::cout << str1.c_str() << std::endl;
}





