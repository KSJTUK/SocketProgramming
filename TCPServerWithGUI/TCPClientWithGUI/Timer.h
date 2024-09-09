#pragma once

/* ----------------------------------------
*					Timer
*
* Frame Time, FPS�� ����
  ---------------------------------------- */

class Timer {
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = Clock::time_point;

public:
	Timer();
	~Timer();

private:
	TimePoint prev;
};