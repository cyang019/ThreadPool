#ifndef LEAN_THREADPOOL_H
#define LEAN_THREADPOOL_H

#include <future>       // packaged_task
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <queue>
#include <vector>
#include <memory>



namespace lean {
  inline namespace v1 {
    template<typename T>
    class ThreadPool {
    public:
        ThreadPool();
        ThreadPool(std::uint64_t n_workers);
        ~ThreadPool();

        ThreadPool<T>& add_task(std::function<T()>&&);

        ThreadPool<T>& run();
        std::vector<T> get_results();
    private:
        mutable std::mutex m_mu;
        mutable std::condition_variable m_cv;
        bool m_done;
        std::queue<std::function<T()>> m_job_q;
        std::vector<std::unique_ptr<std::thread>> m_workers;
        std::vector<std::future<std::vector<T>>> m_result_getters;

        std::vector<T> m_work()
    };  // class ThreadPool
  } // v1
} // lean

#include "threadpool_impl.hpp"

#endif
