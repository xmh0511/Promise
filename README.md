# c++ Promise library
特点：
Promise 用起来很像js的Promise库
c++11 标准就能支持
## 如何使用

````int main(){
   xmh::get_promise().then([](){
      std::cout<<"hello world"<<std::endl;
   });
}
````
