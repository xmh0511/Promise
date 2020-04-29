# c++ Promise library
特点：
Promise 用起来很像js的Promise库
c++11 标准就能支持
## 如何使用

# 链式写法

````
	xmh::Promise([](auto resolve, auto reject) {
		  std::thread([resolve]() {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            resolve(1000);
			}).detach();
	}).then([](int size) {
			std::cout << size << std::endl;
			return xmh::Promise([](auto resolve, auto reject) {
				resolve(Test{ 10.23 
         });
	}).then([](Test vv) {
			return xmh::Promise([vv](auto resolve, auto reject) {
						resolve(vv.c);
						});
			});
	}).then([](double k) {
			std::cout << k << std::endl;
	});
````
