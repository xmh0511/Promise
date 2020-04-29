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
		template<typename U>
		any_data(U&& v) :value_(v) {

		}
		T value_;
	};
	struct any {
		any() = default;
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
	struct Resolver {
		Resolver(T* that) :that_(that) {

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
	struct Rejecter {
		Rejecter(T* that) :that_(that) {

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

	struct promise_implementation {
		promise_implementation() = default;
		template<typename Task>
		promise_implementation(Task&& task) :task_snapshot_(std::bind(std::forward<Task>(task), Resolver<promise_implementation>{ this }, Rejecter<promise_implementation>{this})) {

		}

		template<typename...Args>
		void resolve(Args&& ...args) {
			auto tp = std::make_tuple(args...);
			promise_.set_value(tp);
		}

		template<typename...Args>
		void reject(Args&& ...args) {

		}

		template<typename Function, typename Tuple, std::size_t...Indexs>
		auto deference(Function&& exectuor, Tuple&& tup, std::index_sequence<Indexs...>) {
			return exectuor(std::get<Indexs>(tup)...);
		}

		template<typename Function>
		auto then(Function&& exectuor) {
			task_snapshot_();
			auto future = promise_.get_future();
			auto v = future.get();
			using traits = function_traits<typename std::remove_reference<Function>::type>;
			using params_type = typename traits::non_reference_params_type;
			return deference(std::forward<Function>(exectuor), any_cast<params_type>(v), std::make_index_sequence<traits::args_size>{});
		}

	private:
		std::function<void()> task_snapshot_;
		std::promise<any> promise_;
	};
	class Promise {
	public:
		Promise() = default;
		Promise(Promise const& p) :promise_(p.promise_) {

		}
		template<typename Task, typename U = typename std::enable_if<!std::is_same<std::remove_reference_t<Task>, Promise>::value>::type>
		Promise(Task&& task) : promise_(std::make_shared<promise_implementation>(std::forward<Task>(task))) {
		}
	public:
		template<typename Function>
		auto then(Function&& exectuor) {
			return promise_->then(std::forward<Function>(exectuor));
		}
	private:
		std::shared_ptr<promise_implementation> promise_;
	};
}
