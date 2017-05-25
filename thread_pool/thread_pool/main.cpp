#include <iostream>
#include "thread_pool.h"
#include <thread>

using namespace std;
std::mutex x;

void foo()
{
	for (int i = 0; i < 100; ++i)
	{
		std::lock_guard<std::mutex> lk(x);
        std::cout<<"Thread "<<std::this_thread::get_id()<<" no.:"<<i<<std::endl;
	}
}

int main(int argc, char *argv[])
{

    thread_pool tp;
	std::vector<std::future<void>> res;
	for (unsigned i = 0; i < 10; ++i)
	{
		res.push_back(std::move(tp.submit(foo)));
	}
	for (auto it = res.begin(); it != res.end(); ++it)
		it->get();
    return 0;
}
