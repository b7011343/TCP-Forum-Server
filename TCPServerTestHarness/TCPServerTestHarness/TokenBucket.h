#pragma once

#include <atomic>
#include <chrono>

class TokenBucket {
public:
	TokenBucket() {}

	TokenBucket(const uint64_t rate, const uint64_t burstSize) {
		timePerToken_ = 1000000 / rate;
		timePerBurst_ = burstSize * timePerToken_;
	}

	TokenBucket(const TokenBucket& other) {
		timePerToken_ = other.timePerToken_.load();
		timePerBurst_ = other.timePerBurst_.load();
	}

	TokenBucket& operator=(const TokenBucket& other) {
		timePerToken_ = other.timePerToken_.load();
		timePerBurst_ = other.timePerBurst_.load();
		return *this;
	}

	bool consume(const uint64_t tokens) {
		const uint64_t now =
			std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now().time_since_epoch())
			.count();
		const uint64_t timeNeeded =
			tokens * timePerToken_.load(std::memory_order_relaxed);
		const uint64_t minTime =
			now - timePerBurst_.load(std::memory_order_relaxed);
		uint64_t oldTime = time_.load(std::memory_order_relaxed);
		uint64_t newTime = oldTime;

		if (minTime > oldTime) {
			newTime = minTime;
		}

		for (;;) {
			newTime += timeNeeded;
			if (newTime > now) {
				return false;
			}
			if (time_.compare_exchange_weak(oldTime, newTime,
				std::memory_order_relaxed,
				std::memory_order_relaxed)) {
				return true;
			}
			newTime = oldTime;
		}

		return false;
	}

private:
	std::atomic<uint64_t> time_ = { 0 };
	std::atomic<uint64_t> timePerToken_ = { 0 };
	std::atomic<uint64_t> timePerBurst_ = { 0 };
};
