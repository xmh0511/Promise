#pragma once
#include <thread>
#include <condition_variable>
#include <tuple>
#include <functional>
#include <mutex>
#include <memory>
#include <string>
namespace xmh {
	template<typename...Args>
	class promise;
	template<typename...Args>
	class promise_co
	{
	public:
		promise_co() {
			promise_ = std::make_shared<promise<Args...>>();
		}
		template<typename...Params>
		void resolve(Params&&...params) const {
			promise_->resolve(std::forward<Params>(params)...);
		}
		template<typename Lambda>
		auto then(Lambda&& function) const {
			return promise_->then(std::forward<Lambda>(function));
		}
	private:
		mutable std::shared_ptr<promise<Args...>> promise_;
	};

	enum  class promise_state :std::uint32_t
	{
		init,
		pending,
		resolve,
		reject,
	};

	template<typename...Args>
	class promise
	{
		//template<typename T>
		//friend class promise_co<T>;
	public:
		promise() {
			state_ = promise_state::init;
		}
	public:
		template<typename...Params>
		void resolve(Params&&...params) {
			//std::cout << typeid(arguments_).name() << "   " << sizeof...(params)<<"   "<<typeid(tp).name() << std::endl;
			arguments_ = std::tuple<Args...>(static_cast<Args>(params)...);
			resolvet_ = std::thread([that = this]() mutable {
				std::unique_lock<std::mutex> lck(that->other_mutex_);
				that->other_cv_.wait(lck,[that]() {
					return that->state_ == promise_state::pending;
				});
				if (that->state_ == promise_state::pending) {
					that->state_ = promise_state::resolve;
					that->cv.notify_one();
				}
			});
			resolvet_.detach();
		}
		template<typename Lambda>
		auto then(Lambda&& function) {
			std::unique_lock<std::mutex> lck(mutex_);
			{
				std::unique_lock<std::mutex> lck0(other_mutex_);
				state_ = promise_state::pending;
			}
			other_cv_.notify_one();
			cv.wait(lck, [this]() {
				return this->state_ == promise_state::resolve;
			});
			return apply(std::forward<Lambda>(function), std::make_index_sequence<sizeof...(Args)>{});
		}
	protected:
		template<std::size_t...Indexs, typename Lambda>
		auto apply(Lambda&& function, std::index_sequence<Indexs...>) {
			return function(std::get<Indexs>(arguments_)...);
		}
	private:
		std::tuple<Args...> arguments_;
		std::mutex mutex_;
		std::condition_variable cv;
		promise_state state_;
		std::thread resolvet_;
		std::mutex other_mutex_;
		std::condition_variable other_cv_;
	};

	template<>
	class promise<>
	{
	public:
		promise() = default;
		template<typename Lambda>
		auto then(Lambda&& function) {
			auto pm = function();
			return pm;
		}
	};

	promise<> get_promise() {
		return promise<>{};
	}
}
#define GO(...) std::thread([]{ __VA_ARGS__ }).detach();