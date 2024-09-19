#pragma once

class Timer {
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;
	using TimeUnit = std::chrono::milliseconds;

public:
	Timer(); 
	~Timer();

public:
	const float GetTimeElapsed() const;
	const float GetDeltaTime() const;

private:
	TimePoint mPrev{ };
	float mTimeElapsed{ };
	float mDeltaTime{ };
	int mFPS{ };
};