#pragma once

#include <windows.h>

class Timer
{
public:
	Timer();
	void StartWatch();
	void StopWatch();
	float GetTimeMilli() const;
private:
	float invFreqMilli;
	bool watchStopped;
	unsigned long long currentCount;
	unsigned long long startCount;
};