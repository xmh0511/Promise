#include <iostream>
#include "promise.hpp"
int main()
{
	auto c = promise_co().then([](auto promise){
	    return promise->resolve(20,20.12);
	}).then([](auto promise,int a,double b) {
		std::cout << a <<"   "<< b << std::endl;
		return promise->resolve(std::string("OK"),20);
	}).then([](auto promise, auto str,auto i) {
		std::cout << str << "   " << i << std::endl;
		return promise->resolve(std::string("OK"), 20);
	});

	c.then([](auto promise,auto str,auto b) {
		std::cout << str << "   " << b << std::endl;
		return promise->resolve();
	});


	std::cin.get();
	return 0;
}
