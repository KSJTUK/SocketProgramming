#include "pch.h"
#include "Timer.h"

/* ----------------------------------------
*
*					Timer
*
  ---------------------------------------- */

Timer::Timer()
{
	mPrev = Clock::now();
}

Timer::~Timer()
{
}

void Timer::Update()
{
	++mFpsCount;

	TimePoint current = Clock::now();

	mDeltaTime = std::chrono::duration<TimeType>(current - mPrev).count();
	mPrev = std::move(current);

	mElapsedTime += mDeltaTime;
	mFrameCounter += mDeltaTime;
	// 1초가 지나면 fps 갱신
	if (mFrameCounter > 1.0f) {
		mFps = mFpsCount;

		mFrameCounter = 0;
		mFpsCount = 0;
	}
}

int Timer::GetFPS() const
{
	return mFps;
}

Timer::TimeType Timer::GetDeltaTime() const
{
	return mDeltaTime;
}

Timer::TimePoint Timer::GetCurrentTick() const
{
	return Clock::now();
}
