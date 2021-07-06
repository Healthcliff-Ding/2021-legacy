#include "vector.hpp"

#include <iostream>

#include <vector>

int main()	// 也没有内存泄漏
{
	sjtu::vector<long long> v;
	for (long long i = 0; i < 1LL << 10; ++i) {
		v.push_back(i);
	}
	std::cout << v.back() << std::endl;
	for (long long i = 0; i < 1LL << 10; ++i) {
		v.insert(v.begin(), i);
	}
	for (size_t i = 0; i < 1LL << 10; ++i) {	
		std::cout << v.front() << std::endl;
		v.erase(v.begin());
	}
	return 0;
}
