#pragma once
#include <iostream>
#include <tuple>
#include <string>
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

template<typename...Args>
struct is_promise_type<Promise<Args...>&>
{
	static constexpr bool value = true;
};

template<typename...Args>
struct is_promise_type<Promise<Args...>&&>
{
	static constexpr bool value = true;
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
	Promise(Params&&...params):args_tuple(std::forward<Params>(params)...)
	{

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
		auto cb =  then_call(std::forward<Function>(function), std::make_index_sequence<sizeof...(Args)>{});
		while (status_ == promise_state::pending)
		{

		}
		return cb;
	}
public:
	template<typename...Params>
	auto resolve(Params&&...args)->Promise<Params...>
	{
		status_ = promise_state::fulfilled;
		return Promise<Params...>(std::forward<Params>(args)...);
	}
private:
	template<typename Function,std::size_t...Indexs>
	auto then_call(Function&& function,std::index_sequence<Indexs...>)
	{
		return function(this,std::get<Indexs>(args_tuple)...);
	}

private:
	std::tuple<Args...> args_tuple;
	promise_state status_ = promise_state::pending;
};

template<typename...Args>
auto call_back(Args&&...args)->Promise<Args...>
{
	return Promise<Args...>(std::forward<Args>(args)...);
}

using promise_co = Promise<>;
