#include <iostream>
#include <utility>
#include <thread>
#include <vector>
#include <chrono>

#include "share_ptr.h"

void test_myshare_ptr()
{
    myshare_ptr<int> ptr(new int(1024));
    std::vector<std::thread> tvec;
    std::size_t thread_num = 10;
    for (int i = 0; i < thread_num ; ++i)
    {
        tvec.emplace_back([&ptr](){
            for (int j = 0; j < 10000; ++j)
            {
                myshare_ptr<int> tmp_sp = ptr;
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }

    for (auto& t : tvec)
    {
        t.join();
    }

    std::cout << ptr.get_ref_count() << std::endl;
} 


int main()
{
    test_myshare_ptr();
    return 0;
}