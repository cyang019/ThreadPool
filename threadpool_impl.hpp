namespace lean {
	inline namespace v1 {
		template<typename T>
		ThreadPool<T>::ThreadPool()
			: m_done(false), m_worker_cnt(std::thread::hardware_concurrency())
		{
		}

		template<typename T>
		ThreadPool<T>::ThreadPool(std::uint64_t n_workers)
			: m_done(false), m_worker_cnt(n_workers)
		{
		}

		template<typename T>
		ThreadPool<T>::~ThreadPool()
		{
			{
				std::lock_guard<std::mutex> lk(m_mu);
				m_done = true;
				while (!m_job_q.empty()) {
					m_job_q.pop();
				}
			}
			
			for (auto& t : m_workers) {
				t.join();
			}
		}

		template<typename T>
		ThreadPool<T>& ThreadPool<T>::add_task(std::function<T()>&& task)
		{
			{
				std::lock_guard<std::mutex> lk(m_mu);
				m_job_q.push(std::move(task));
			}
			
			return *this;
		}

		template<typename T>
		ThreadPool<T>& ThreadPool<T>::run()
		{
			for (std::uint64_t i = 0; i < m_worker_cnt; ++i) {
				std::packaged_task<std::vector<T>()> worker([this]() {
					std::vector<T> results;
					while (true) {
						std::unique_lock<std::mutex> lk(m_mu);
						m_cv.wait(lk, [this]() {
							return this->m_done || (!m_job_q.empty());
						});

						if (this->m_done && m_job_q.empty()) {
							m_cv.notify_one();
							break;
						}

						std::function<T()> task = std::move(m_job_q.front());
						m_job_q.pop();
						m_cv.notify_one();
						lk.unlock();

						T res = task();
						results.push_back(std::move(res));
					}
					return results;
				});
				m_result_getters.emplace_back(worker.get_future());
				m_workers.emplace_back(std::move(worker));
			}

			return *this;
		}

		template<typename T>
		std::vector<T> ThreadPool<T>::get_results()
		{
			std::vector<T> results;
			{
				std::unique_lock<std::mutex> lk(m_mu);
				m_cv.wait(lk, [this]() {
					return m_job_q.empty();
					});
				m_done = true;
				m_cv.notify_one();
			}

			for (auto& result_getter : m_result_getters) {
				auto res = result_getter.get();
				results.insert(results.end(), res.begin(), res.end());
			}
			return results;
		}
	}
}