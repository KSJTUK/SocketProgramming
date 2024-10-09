#pragma once

/* ----------------------------------------
*					Timer
*
* Frame Time, FPSµî ÃøÁ¤
  ---------------------------------------- */

class Timer {
public:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;
	using DefaultTimeUnit = std::chrono::seconds;
	using TimeType = float;

public:
	Timer();
	~Timer();

public:
	void Update();
	int GetFPS() const;
	TimeType GetDeltaTime() const;
	TimePoint GetCurrentTick() const;

private:
	TimePoint mPrev{ };
	TimeType mDeltaTime{ };
	TimeType mElapsedTime{ };
	TimeType mFrameCounter{ };
	int mFpsCount{ };
	int mFps{ };
};