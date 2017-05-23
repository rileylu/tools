#include "thread_pool.h"
#include <iostream>
void hello()
{
	for(int i=0;i<10;++i)
		std::cout << "Thread " << std::this_thread::get_id() << " : Hello World" << std::endl;
}

int main(int argc, char *argv[])
{
	thread_pool p;
	std::vector<std::future<void>> lst;
	for (int i = 0; i < 5; ++i)
	{
		lst.push_back(std::move(p.submit(hello)));
	}
	for (auto it = lst.begin(); it != lst.end(); ++it)
		it->get();
    return 0;
}
