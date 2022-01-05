#pragma once
#include <tuple>
#include <functional>
#include <future>
#include <memory>
namespace xmh {
	template<typename T, typename U = void>
	struct function_traits;
	template<typename Ret, typename...Args>
	struct function_traits<Ret(*)(Args...)> {
		using ret_type = Ret;
		using parameters_type = std::tuple<Args...>;
		using non_reference_params_type = std::tuple<typename std::remove_reference<Args>::type...>;
		static constexpr std::size_t args_size = sizeof...(Args);
	};
	template<typename Ret, typename Class, typename...Args>
	struct function_traits<Ret(Class::*)(Args...) const> {
		using ret_type = Ret;
		using parameters_type = std::tuple<Args...>;
		using non_reference_params_type = std::tuple<typename std::remove_reference<Args>::type...>;
		static constexpr std::size_t args_size = sizeof...(Args);
	};
	template<typename T>
	struct function_traits<T, std::void_t<decltype(&T::operator())>> :function_traits<decltype(&T::operator())> {

	};
	struct any_data_hold {
		virtual ~any_data_hold() {

		}
	};
	template<typename T>
	struct any_data :any_data_hold {
		any_data(T const& v) :value_(v) {

		}
		T value_;
	};
	struct any {
		any() = default;
		any(any const&) = default;
		template<typename T, typename U = typename std::enable_if<!std::is_same<any, typename std::remove_reference<T>::type>::value>::type>
		any(T&& value) :holder_(std::make_shared<any_data<typename std::remove_reference<T>::type>>(std::forward<T>(value))) {

		}
		std::shared_ptr<any_data_hold> holder_;
	};

	template<typename T>
	T any_cast(any const& v) {
		auto r = std::dynamic_pointer_cast<any_data<T>>(v.holder_);
		if (r) {
			return r->value_;
		}
		throw T{};
	}


	template<typename T>
	struct Resolver_ {
		Resolver_(T* that) :that_(that) {

		}
		template<typename...U>
		void operator()(U&& ...value) const {
			if (that_) {
				that_->resolve(std::forward<U>(value)...);
			}
		}
	private:
		T* that_ = nullptr;
	};
	template<typename T>
	struct Rejecter_ {
		Rejecter_(T* that) :that_(that) {

		}
		template<typename...U>
		void operator()(U&& ...value) const {
			if (that_) {
				that_->reject(std::forward<U>(value)...);
			}
		}
	private:
		T* that_ = nullptr;
	};
	class Promise;
	enum class promise_state {
		pending,
		resolve,
		reject
	};
	struct promise_implementation {
		promise_implementation() = default;
		promise_implementation(promise_implementation const& p) :task_snapshot_(p.task_snapshot_), state_(p.state_){

		}
		template<typename Task>
		promise_implementation(Task const& task) : task_snapshot_(std::bind(task, Resolver_<promise_implementation>{ this }, Rejecter_<promise_implementation>{this})) {

		}

		template<typename...Args>
		void resolve(Args&& ...args) {
			auto tp = std::make_tuple(args...);
			state_ = promise_state::resolve;
			promise_.set_value(tp);
		}

		template<typename...Args>
		void reject(Args&& ...args) {
			auto tp = std::make_tuple(args...);
			state_ = promise_state::reject;
			promise_.set_value(tp);
		}

		template<typename Ret,typename Function,typename Tuple, std::size_t...Indexs>
		auto deference(Function&& exectuor, Tuple&& tup, std::index_sequence<Indexs...>)->typename std::enable_if<!std::is_same<void, Ret>::value, Promise>::type {
			return exectuor(std::get<Indexs>(tup)...);
		}

		template<typename Ret,typename Function,typename Tuple, std::size_t...Indexs>
		auto deference(Function&& exectuor, Tuple&& tup, std::index_sequence<Indexs...>)->typename std::enable_if<std::is_same<void, Ret>::value, Promise>::type {
			exectuor(std::get<Indexs>(tup)...);
			return *this;
		}

		template<class P,typename Function>
		P then(Function&& resolve) {
			if (state_ != promise_state::pending) {
				return *this;
			}
			task_snapshot_();
			auto future = promise_.get_future();
			auto v = future.get();
			using traits = function_traits<typename std::remove_reference<Function>::type>;
			using params_type = typename traits::non_reference_params_type;
			if (state_ == promise_state::resolve) {
				return deference<typename traits::ret_type>(std::forward<Function>(resolve), any_cast<params_type>(v), std::make_index_sequence<traits::args_size>{});
			}
			return *this;
		}

		template<class P,typename Resolve,typename Reject>
		P then(Resolve&& resolve, Reject&& reject) {
			if (state_ != promise_state::pending) {
				return *this;
			}
			task_snapshot_();
			auto future = promise_.get_future();
			auto v = future.get();
			if (state_ == promise_state::resolve) {
				using traits = function_traits<typename std::remove_reference<Resolve>::type>;
				using params_type = typename traits::non_reference_params_type;
				return deference<typename traits::ret_type>(std::forward<Resolve>(resolve), any_cast<params_type>(v), std::make_index_sequence<traits::args_size>{});
			}
			else if (state_ == promise_state::reject) {
				using traits = function_traits<typename std::remove_reference<Reject>::type>;
				using params_type = typename traits::non_reference_params_type;
				return deference<typename traits::ret_type>(std::forward<Reject>(reject), any_cast<params_type>(v), std::make_index_sequence<traits::args_size>{});
			}
			return *this;
		}


	private:
		std::function<void()> task_snapshot_;
		std::promise<any> promise_;
		promise_state state_ = promise_state::pending;
	};
	class Promise {
	public:
		Promise() = default;
		Promise(Promise const& p) :promise_(p.promise_) {

		}
		Promise(promise_implementation const& v) :promise_(new promise_implementation{v}) {

		}
		template<typename Task>
		Promise(Task const& task) : promise_(std::make_shared<promise_implementation>(task)) {
		}
	public:
		template<typename Resolve,typename...Reject>
		auto then(Resolve&& exectuor, Reject&&...reject) {
			return promise_->then<Promise>(std::forward<Resolve>(exectuor), std::forward<Reject>(reject)...);
		}
	private:
		std::shared_ptr<promise_implementation> promise_;
	};
	using Resolve = Resolver_<promise_implementation>;
	using Reject = Rejecter_<promise_implementation>;
}
