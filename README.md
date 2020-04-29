# c++ Promise library
特点：
Promise 用起来很像js的Promise库
c++11 标准就能支持
## 如何使用

# 链式写法

````cpp
#include "promise.hpp"
int main(){
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
}
````
