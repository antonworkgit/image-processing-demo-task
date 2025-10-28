#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class SPSC_DataBuffer
{
public:
	explicit SPSC_DataBuffer(size_t capacity);

	void push(T value);
	bool try_pop(T& value);

	/// <summary>
	/// Signals the buffer that the producer has finished its last addition and no more items will be added.
	/// </summary>
	void finish_writing();

	/// <summary>
	/// Signals the buffer that all current and future pop/push operations should be cancelled.
	/// </summary>
	void cancel_operations();

	bool is_completed() const;
	bool is_cancelled() const;

private:
	size_t capacity_;
	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cv_empty_;
	std::condition_variable cv_full_;
	std::atomic<bool> completed_{ false }; // Indicates that pushing to the buffer is completed. Popping can continue until the last element.
	std::atomic<bool> cancelled_{ false }; // Indicates that both pushing and popping are cancelled. No push or pop operations are allowed.
};

template<typename T>
SPSC_DataBuffer<T>::SPSC_DataBuffer(size_t capacity) : capacity_(capacity) {}

template<typename T>
void SPSC_DataBuffer<T>::push(T value) {
	std::unique_lock<std::mutex> lock(mutex_);

	cv_full_.wait(lock, [this] { return queue_.size() < capacity_ || completed_ || cancelled_; });

	if (completed_ || cancelled_) {
		return;
	}

	queue_.push(std::move(value));
	// std::cout << "\nq_push_" << typeid(T).name() << '\n';
	cv_empty_.notify_one();
}

template<typename T>
bool SPSC_DataBuffer<T>::try_pop(T& value) {
	std::unique_lock<std::mutex> lock(mutex_);

	cv_empty_.wait(lock, [this] { return !queue_.empty() || completed_ || cancelled_; });

	if (queue_.empty() || cancelled_) {
		return false;
	}

	value = std::move(queue_.front());
	queue_.pop();
	// std::cout << "\nq_pop_" << typeid(T).name() << '\n';
	cv_full_.notify_one();
	return true;
}

template<typename T>
bool SPSC_DataBuffer<T>::is_completed() const {
	return completed_.load();
}

template<typename T>
bool SPSC_DataBuffer<T>::is_cancelled() const {
	return cancelled_.load();
}

template<typename T>
void SPSC_DataBuffer<T>::finish_writing() {
	completed_.store(true);
	cv_empty_.notify_all();
	cv_full_.notify_all();
}

template<typename T>
void SPSC_DataBuffer<T>::cancel_operations() {
	cancelled_.store(true);
	cv_empty_.notify_all();
	cv_full_.notify_all();
}