#include <iostream>
#include "promise.hpp"
#include <chrono>
#include <thread>
int main()
{
	 auto c = promise_co<>().then([](){
		auto promise = promise_co<int, double>::create();
		//promise.resolve(20, 20.12);
		auto point = std::chrono::steady_clock::now();
		std::thread t([promise, point]() mutable {
			while (true) {
				auto now = std::chrono::steady_clock::now();
				auto distance = now - point;
				auto t = std::chrono::duration_cast<std::chrono::duration<long, std::ratio<1, 1>>>(distance).count();
				//std::cout << t << std::endl;
				if (t > 5) {
					break;
				}
			}
			promise.resolve(20, 20.12);
		});
		t.detach();
		return promise;
	 }).then([](int a, double b) {
		auto promise = promise_co<std::string,int>::create();
		std::cout << a << "  " << b << std::endl;
		promise.resolve(std::string("OK"), 20);
		return promise;
	}).then([](auto str,auto i) {
		auto promise = promise_co<double,std::string>::create();
		std::cout << str << "   " << i << std::endl;
		promise.resolve(12.03, std::string("end"));
		return promise;
	});

	c.then([](auto str,auto b) {
		auto promise = promise_co<>::create();
		std::cout << str << "   " << b << std::endl;
		return promise;
	});

	std::cin.get();
	return 0;
}
