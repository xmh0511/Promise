#include <iostream>
#include <thread>
#include "promise.hpp"
#include <random>
struct Test {
	int c;
};
int main() {
	//xmh::Promise([](auto resolve, auto reject) {
	//	std::thread([resolve]() {
	//		std::this_thread::sleep_for(std::chrono::seconds(3));
	//		resolve(1000);
	//		}).detach();
	//	}).then([](int size) {
	//		std::cout << size << std::endl;

	//	});
			//.then([](double k) {
			//	std::cout << k << std::endl;
			//	});
	//xmh::Promise([](auto resolve, auto reject) {
	//	resolve(0);
	//}).then([](int a) {

	//});

	 xmh::Promise([](auto resolve, auto reject) {
		 std::random_device rd;
		 std::mt19937 mt(rd());
		 std::uniform_int_distribution<int> ud(0, 9);
		 auto r = ud(mt);
		 Test v = Test{ r };
		 resolve(v);
	}).then([](Test vv) {
		return xmh::Promise([vv](auto resolve, auto reject) {
			if (vv.c <= 5) {
				reject(char(vv.c));
			}
			else {
				resolve(vv.c);
			}
		});
	}).then([](int v) {
		return xmh::Promise([v](auto resolve, auto reject) {
			std::thread([v, resolve]() {
				std::this_thread::sleep_for(std::chrono::seconds(2));
				std::cout << "resolve " << v << std::endl;
				resolve(v);
			}).detach();
		});
	}, [](char c) {
		std::cout << int(c) << std::endl;
	}).then([](int d) {
		std::cout << "end "<<d << std::endl;
	});
	std::getchar();
}
