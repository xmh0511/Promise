#pragma once
#include <iostream>
#include <tuple>
#include <string>
#include <memory>
template<typename...Args>
class Promise;

template<typename T>
struct is_promise_type
{
	static constexpr bool value = false;
};

template<typename...Args>
struct is_promise_type<Promise<Args...>>
{
	static constexpr bool value = true;
};

//template<typename...Args>
//struct is_promise_type<Promise<Args...>&>
//{
//	static constexpr bool value = true;
//};
//
//template<typename...Args>
//struct is_promise_type<Promise<Args...>&&>
//{
//	static constexpr bool value = true;
//};

template<typename...Args>
struct context
{

};

template<typename...Args>
struct has_no_promise
{
	
};

template<typename T,typename...Args>
struct has_no_promise<T,Args...>
{
	static constexpr bool value = !is_promise_type<T>::value && has_no_promise<Args...>::value;
};

template<>
struct has_no_promise<>
{
	static constexpr bool value = true;
};

class PromiseBase
{
public:
	virtual ~PromiseBase() = default;
};

enum class promise_state
{
	pending,
	fulfilled,
	rejected
};
template<typename...Args>
class Promise:public PromiseBase
{
public:
	Promise() = default;

	template<typename...Params,typename  = std::enable_if_t<has_no_promise<Params...>::value>>
	Promise(Params&&...params) //:args_tuple(std::forward<Params>(params)...)
	{
		std::cout << typeid(context<Params...>).name() << std::endl;
	}

	Promise(Promise const& p):args_tuple(p.args_tuple)
	{
		//std::cout << "copy construct" << std::endl;
	}

	Promise& operator=(Promise const& p)
	{
		args_tuple = p.args_tuple;
		return *this;
	}

	~Promise()
	{
		//std::cout << "Promise destory" << std::endl;
	}
public:
	template<typename Function>
	auto then(Function&& function)
	{
		while (status_ == promise_state::pending)
		{

		}
		auto cb =  then_call(std::forward<Function>(function), std::make_index_sequence<sizeof...(Args)>{});
		return cb;
	}
public:
	template<typename...Params>
	auto resolve(Params&&...args)
	{
		args_tuple = std::tuple<Args...>(args...);
		status_ = promise_state::fulfilled;
	}

	static auto create()
	{
		return std::make_shared<Promise>();
	}
private:
	template<typename Function,std::size_t...Indexs>
	auto then_call(Function&& function,std::index_sequence<Indexs...>)
	{
		return function(std::get<Indexs>(args_tuple)...);
	}

private:
	std::tuple<Args...> args_tuple;
	promise_state status_ = promise_state::pending;
};

template<>
class Promise<> :public PromiseBase
{
public:
	Promise() = default;
public:
	template<typename Function>
	auto then(Function&& function)
	{
		auto cb = function();
		return cb;
	}
	static auto create()
	{
		return std::make_shared<Promise>();
	}
};

template<typename...Args>
class promise_co
{
public:
	promise_co() = default;

	promise_co(promise_co const& p):co_(p.co_)
	{

	}
public:
	template<typename Function>
	auto then(Function&& function)
	{
		return co_->then(std::forward<Function>(function));
	}

	template<typename...Params>
	auto resolve(Params&&...args)
	{
		return co_->resolve(std::forward<Params>(args)...);
	}

	static auto create()
	{
		return promise_co{};
	}
private:
	std::shared_ptr<Promise<Args...>> co_ = std::make_shared<Promise<Args...>>();
};

template<typename...Args>
auto call_back(Args&&...args)->Promise<Args...>
{
	return Promise<Args...>(std::forward<Args>(args)...);
}

