#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "threadsafe_queue.h"
#include "join_threads.h"
#include "function_wrapper.h"
#include "work_stealing_queue.h"
#include <atomic>
#include <vector>
#include <thread>
#include <future>
#include <functional>
#include <iostream>

class thread_pool
{
    typedef function_wrapper task_type;
    typedef std::vector<std::unique_ptr<work_stealing_queue>> queues_vector;

    unsigned const thread_count;
    std::atomic_bool done;
    threadsafe_queue<task_type> pool_work_queue;
    queues_vector const queues;

    std::vector<std::thread> threads;
    join_threads joiner;

    static thread_local work_stealing_queue* local_work_queue;
    static thread_local unsigned my_index;

    queues_vector make_queues()
    {
        queues_vector queues;
        queues.reserve(thread_count);
        for(unsigned i=0;i<thread_count;++i)
        {
            queues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue));
        }
        return queues;
    }

    void worker_thread(unsigned my_index_)
    {
        my_index=my_index_;
        local_work_queue=queues[my_index].get();

        while (!done)
        {
            run_pending_task();
        }
    }

    bool pop_task_from_local_queue(task_type& task)
    {
        return local_work_queue && local_work_queue->try_pop(task);
    }

    bool pop_task_from_pool_queue(task_type& task)
    {
        return pool_work_queue.try_pop(task);
    }

    bool pop_task_from_other_thread_queue(task_type& task)
    {
        for(unsigned i=0;i<queues.size();++i)
        {
            unsigned const index=(my_index+i+1)%queues.size();
            if(queues[index]->try_steal(task))
            {
                return true;
            }
        }
        return false;
    }

public:
    thread_pool() :
        thread_count(std::thread::hardware_concurrency()),
        done(false),
        queues(make_queues()),
        joiner(threads)
    {
        unsigned const thread_count = std::thread::hardware_concurrency();
        try
        {
            for (unsigned i = 0; i < thread_count; ++i)
            {
                threads.push_back(std::thread(&thread_pool::worker_thread, this,i));
            }
        }
        catch (...)
        {
            done = true;
            throw;
        }
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        if(local_work_queue)
        {
            local_work_queue->push(std::move(task));
        }
        else
        {
            pool_work_queue.push(std::move(task));
        }
        return res;
    }

    void run_pending_task()
    {
        task_type task;
        if(pop_task_from_local_queue(task)||
                pop_task_from_pool_queue(task)||
                pop_task_from_other_thread_queue(task))
        {
            task();
        }
        else
        {
            std::this_thread::yield();
        }
    }

    ~thread_pool()
    {
        done = true;
    }
};


thread_local work_stealing_queue* thread_pool::local_work_queue=nullptr;
thread_local unsigned thread_pool::my_index=-1;
#endif // THREAD_POOL_H
