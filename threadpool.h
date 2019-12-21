#ifndef LEAN_THREADPOOL_H
#define LEAN_THREADPOOL_H

#include <future>       // packaged_task
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdint>
#include <queue>
#include <vector>



namespace lean {
  inline namespace v1 {
    template<typename T>
    class ThreadPool {
    public:
    private:
        mutable std::mutex m_mu;
        mutable std::condition_variable m_cv;
        std::uint64_t m_cnt;
        bool m_done;
        std::queue<T> m_job_q;
    };  // class ThreadPool
  } // v1
} // lean


#endif
