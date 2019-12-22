#include "threadpool.h"
#include <limits>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <cmath>


double calcSquare(double val)
{
	return val * val;
}


int main()
{
	std::vector<double> inputs = { 
		1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 
		5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 8.5 };

	std::vector<double> results1;
	for (const auto& input : inputs) {
		results1.push_back(calcSquare(input));
	}
	std::cout << "Results1: \n";
	for (const auto& r1 : results1) {
		std::cout << r1 << " ";
	}
	std::cout << "\n" << std::endl;

	lean::ThreadPool<double> tpool(4);
	for (const auto& input : inputs) {
		auto fn = [=]() { return calcSquare(input); };
		tpool.add_task(std::move(fn));
	}
	tpool.run();
	auto results2 = tpool.get_results();

	std::cout << "Results2: \n";
	for (const auto& r2 : results2) {
		std::cout << r2 << " ";
	}
	std::cout << std::endl;

	constexpr int n = 400;
	std::cout << "Repeat the above thread method " << n << " times.\n";
	for (int i = 0; i < n; ++i) {
		lean::ThreadPool<double> tpool(4);
		for (const auto& input : inputs) {
			auto fn = [=]() { return calcSquare(input); };
			tpool.add_task(std::move(fn));
		}
		tpool.run();
		auto results2 = tpool.get_results();
		if (results1.size() != results2.size()) {
			std::cout << "Size differ." << std::endl;
		}
		std::sort(results2.begin(), results2.end());

		constexpr double eps = std::numeric_limits<double>::epsilon();
		for (size_t i = 0; i < results2.size(); ++i) {
			if (std::abs(results1[i] - results2[i]) > eps) {
				std::cout << "\n[" << i << "] " << results1[i]
					<< " vs " << results2[i] << "\n";
			}
		}
	}

	return 0;
}


