#include <iostream>
#include "promise.hpp"
int main() {
	GO(xmh::get_promise().then([](){
		xmh::promise_co<int> pm{};
		std::thread([pm]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			pm.resolve(10);
		}).detach();
		return pm;
	}).then([](int value){
		xmh::promise_co<std::string,double> pm0{};
		std::cout << value << std::endl;
		pm0.resolve(std::string("abc"),20.12);
		return pm0;
	}).then([](std::string name,double age) {
		std::cout << name << " "<<age << std::endl;
	});)

	std::cout << "first command?" << std::endl;
	std::cin.get();
	return 0;
}