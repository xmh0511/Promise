#include <iostream>
#include "promise.hpp"
#include <chrono>
#include <thread>

template<typename Function>
auto setTimeOut(Function&& function,int time)
{
	auto point = std::chrono::steady_clock::now();
	std::shared_ptr<std::thread> t = std::make_shared<std::thread>([function, point, time]() mutable {
		while (true) {
			auto now = std::chrono::steady_clock::now();
			auto distance = now - point;
			auto t = std::chrono::duration_cast<std::chrono::duration<long, std::ratio<1, 1>>>(distance).count();
			//std::cout << t << std::endl;
			if (t > time) {
				function();
				break;
			}
		}
	});
	t->detach();
	return t;
}
int main()
{
	 auto c = promise_co<>().then([](){
		auto promise = promise_co<int, double>::create();
		setTimeOut([promise]() mutable {
			promise.resolve(20, 20.12);
		}, 5);
		return promise;
	 }).then([](int a, double b) {
		auto promise = promise_co<std::string,int>::create();
		std::cout << a << "  " << b << std::endl;
		setTimeOut([promise]() mutable {
			promise.resolve(std::string("OK"), 20);
		}, 2);
		return promise;
	}).then([](auto str,auto i) {
		auto promise = promise_co<double,std::string>::create();
		std::cout << str << "   " << i << std::endl;
		setTimeOut([promise]() mutable {
			promise.resolve(12.03, std::string("end"));
		}, 3);
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
