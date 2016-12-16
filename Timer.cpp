#include "Timer.h"
#pragma comment(lib, "winmm.lib" )

Timer::Timer()
{
	unsigned long long frequency;
	QueryPerformanceFrequency( (LARGE_INTEGER*)&frequency );
	invFreqMilli = 1.0f / (float)((double)frequency / 1000.0);
	StartWatch();
}

void Timer::StopWatch()
{
	if( !watchStopped )
	{
		QueryPerformanceCounter( (LARGE_INTEGER*)&currentCount );
		watchStopped = true;
	}
}

void Timer::StartWatch()
{
	watchStopped = false;
	QueryPerformanceCounter( (LARGE_INTEGER*)&startCount );
}

float Timer::GetTimeMilli() const
{
	if( !watchStopped )
	{
		QueryPerformanceCounter( (LARGE_INTEGER*)&currentCount );
		return (float)(currentCount - startCount) * invFreqMilli;
	}
	else
	{
		return (float)(currentCount - startCount) * invFreqMilli;
	}
}