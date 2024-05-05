#include "Threadpool.h"

#include "../logger/Logger.h"

namespace cppserv {


    void ThreadPool::Init(const size_t numThreads) {
        CPPSERV_TRACE("Initializing thread pool with {} threads", numThreads);
        for (size_t i = 0; i < numThreads; ++i) {
            m_Threads.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_QueueMutex);

                        m_CV.wait(lock, [this] {
                            return !m_Tasks.empty() || m_Stop;
                            });

                        if (m_Stop && m_Tasks.empty()) {
                            return;
                        }

                        task = move(m_Tasks.front());
                        m_Tasks.pop();
                    }

                    task();
                }
                });
        }
    }


    void ThreadPool::Submit(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Tasks.emplace(std::move(task));
        }
        m_CV.notify_one();
    }

    void ThreadPool::Shutdown() {

        CPPSERV_TRACE("Shutting down thread pool");

        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }

        m_CV.notify_all();

        for (auto& thread : m_Threads) {
            thread.join();
        }
    }

    void ThreadPool::Terminate() {

        CPPSERV_TRACE("Terminating thread pool");

        {
            std::unique_lock<std::mutex> lock(m_QueueMutex);
            m_Stop = true;
        }

        m_CV.notify_all();

        for (auto& thread : m_Threads) {
            thread.detach();
        }
    }

} // namespace cppserv

