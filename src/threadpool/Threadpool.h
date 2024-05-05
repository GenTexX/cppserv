#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <condition_variable> 
#include <functional> 
#include <iostream> 
#include <mutex> 
#include <queue> 
#include <thread> 

#include "../core/cppservcore.h"
#include "../socket/Socket.h"

namespace cppserv {
    class ThreadPool {
    public:
        ThreadPool() {}
        ~ThreadPool() {}

        /**
         * \brief Initializes the thread pool with the specified number of threads.
         *
         * This function initializes the thread pool with the specified number of threads.
         *
         * \param numThreads The number of threads to create in the thread pool.
         */
        void Init(const size_t numThreads = std::thread::hardware_concurrency());

        /**
         * \brief Submits a task to the thread pool for execution.
         *
         * This function submits a task to the thread pool for execution.
         *
         * \param task The task to be executed by a thread in the pool.
         */
        void Submit(std::function<void()> task);

        /**
         * \brief Shuts down the thread pool.
         *
         * This function shuts down the thread pool.
         */
        void Shutdown();

        /**
         * \brief Terminates the thread pool.
         *
         * This function terminates the thread pool.
         */
        void Terminate();

    private:
        std::vector<std::thread> m_Threads;
        std::queue<std::function<void()> > m_Tasks;
        std::mutex m_QueueMutex;
        std::condition_variable m_CV;
        bool m_Stop = false;
    };

} // namespace cppserv


#endif // __THREADPOOL_H__