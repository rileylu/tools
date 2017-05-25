#include <iostream>
#include "thread_pool.h"
#include <thread>

using namespace std;

void foo()
{
    for(int i=0;i<10;++i)
        std::cout<<"Thread "<<std::this_thread::get_id()<<" no.:"<<i<<std::endl;
}

int main(int argc, char *argv[])
{

    thread_pool tp;
    for(unsigned i=0;i<10;++i)
        tp.submit(foo);
    return 0;
}
