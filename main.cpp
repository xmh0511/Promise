#include <iostream>
#include "promise.hpp"
int main()
{
	auto c = promise_co().then([](){
		return call_back(20, 20.12);
	}).then([](int a,double b) {
		std::cout << a <<"   "<< b << std::endl;
		return call_back(std::string("OK"),20);
	}).then([](auto str,auto i) {
		std::cout << str << "   " << i << std::endl;
		return call_back(std::string("OK"), 20);
	});

	c.then([](auto str,auto b) {
		std::cout << str << "   " << b << std::endl;
	});

	std::cin.get();
	return 0;
}
