# c++ Promise library
特点：
Promise 用起来很像js的Promise库
c++11 标准就能支持
## 如何使用
# HelloWorld
````
int main(){
   xmh::get_promise().then([](){
      std::cout<<"hello world"<<std::endl;
   });
}
````

# 链式写法

````
  int main(){
   xmh::get_promise().then([](){
      xmh::promise_co<int,double> p{};
      std::cout<<"hello world"<<std::endl;
      return p;
   }).then([](int a,double b){
       std::cout<<a<<" "<<b<<std::endl;
   });
  }
````

# 异步等待
````
   int main(){
     xmh::get_promise().then([](){
        xmh::promise_co<int> p{};
        std::thread([p](){
           std::this_thread::sleep_for(std::chrono::milliseconds(3000));
           p.resolve(10);
        });
        return p;
     }).then([](auto number){
        std::cout<<"get value after 10s,and value = "<< number<<std::endl;
     });
   }
````
