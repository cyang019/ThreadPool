namespace lean {
	inline namespace v1 {
		template<typename T>
		ThreadPool<T>::ThreadPool()
			m_done(true)
		{
			auto n_workers = std::thread::hardware_concurrency();
			m_workers = std::vector<std::unique_ptr<std::thread>>(n_workers, nullptr);
		}

		template<typename T>
		ThreadPool<T>::ThreadPool(std::uint64_t n_workers)
			m_done(true), m_workers(n_workers, nullptr)
		{
		}

		template<typename T>
		ThreadPool<T>::~ThreadPool()
		{
			std::lock_guard<std::mutex> lk(m_mu);
			m_done = true;
			lk.unlock();
			for (auto& uptr_worker : m_workers) {
				upter_worker->join();
			}
		}

		template<typename T>
		ThreadPool<T>& ThreadPool<T>::add_task(std::function<T()>&& task)
		{
			std::lock_guard<std::mutex> lk(m_mu);
			m_job_q.push_back(std::move(task));
			return *this;
		}

		template<typename T>
		ThreadPool<T>& ThreadPool<T>::run()
		{
			std::lock_guard<std::mutex> lk(m_mu);
			m_done = false;
			lk.unlock();
			for (size_t i = 0; i < m_workers.size(); ++i) {
				std::packaged_task<T()> worker(this->m_work);
				m_result_getters.emplace_back(worker.get_future());
				m_workers[i] = std::make_unique(std::move(worker));
			}

			return *this;
		}

		template<typename T>
		std::vector<T> ThreadPool<T>::get_results()
		{
			std::vector<T> results;
			for (auto& result_getter : m_result_getters) {
				auto res = result_getter.get();
				results.insert(results.end(), res.begin(), res.end());
			}
			return results;
		}

		template<typename T>
		std::vector<T> ThreadPool<T>::m_work()
		{
			std::vector<T> results;
			std::unique_lock<std::mutex> lk(m_mu);
			m_cv.wait(lk, [this]() {
				return (!(this->m_done)) || m_job_q.size().empty(); 
			});

			if (this->m_done && m_job_q.size().empty()) {
				return results;
			}
				
			std::function<T()> task = std::move(m_job_q.front());
			m_job_q.pop_front();
			lk.unlock();

			auto res = task();
			results.push_back(res);
		}
	}
}