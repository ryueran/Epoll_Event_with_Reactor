#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <iostream>
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <assert.h>
#include <condition_variable>
#include "Reactor.h"

typedef std::function<void()> Task_type;

class ThreadPool {
public:
    ThreadPool(int thread_num);
    ~ThreadPool();
    void start();
    void stop();
    // void threadLoop();
    void addTask(Reactor* task);
    Reactor* getNextReactor(); //add not free loop to pending loop

private:
    int thread_num_;
    std::vector<std::thread*> m_threads_;
    std::queue<Reactor*> m_tasks_;
    std::mutex m_mutex_;
    std::condition_variable m_cond_;
    bool m_started_;
    Reactor* take_();
};

ThreadPool::ThreadPool(int thread_num)
    :thread_num_(thread_num), m_started_(false)
{
    start();
}

ThreadPool::~ThreadPool()
{
    std::cout << "Thread Pool destrcuted!" << std::endl;
    if(m_started_)
    {
        stop();
    }
}

void ThreadPool::start()
{
    m_started_ = true;
    m_threads_.reserve(thread_num_);
    for(int i = 0; i < thread_num_; i++)
    {
        Reactor *reactor = new Reactor();
        m_threads_.push_back(new std::thread(std::bind(&Reactor::handle_events, reactor)));
        m_tasks_.emplace(reactor);
    }
}

void ThreadPool::stop()
{
    std::unique_lock<std::mutex> lock(m_mutex_);
    m_started_ = false;
    m_cond_.notify_all();

    for(auto it = m_threads_.begin(); it != m_threads_.end(); ++it)
    {
        (*it)->join();
        delete *it;
    }
    m_threads_.clear();
}

void ThreadPool::addTask(Reactor* task)
{
    std::unique_lock<std::mutex> lock(m_mutex_);
    m_tasks_.emplace(task);
    m_cond_.notify_one();
}

// void ThreadPool::threadLoop() // to be deleted
// {
//     while(m_started_)
//     {
//         Reactor* task = take_();
//         if(task)
//         {
//             task->handle_events();
//         }
//     }
// }

Reactor* ThreadPool::take_()
{
    std::unique_lock<std::mutex> lock(m_mutex_);
    while(m_tasks_.empty() && m_started_)
    {
        m_cond_.wait(lock);
    }

    Reactor* task;

    if(!m_tasks_.empty() && m_started_)
    {
        task = m_tasks_.front();
        m_tasks_.pop();
    }

    return task;
}

Reactor* ThreadPool::getNextReactor()
{
    return take_();
}

#endif