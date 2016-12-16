#pragma once

#include "Timer.h"
#include <float.h>

class FrameTimer
{
public:
	FrameTimer()
		:
	timeMin( FLT_MAX ),
	timeMax( 0.0f ),
	timeSum( 0.0f ),
	lastMin( 0.0f ),
	lastMax( 0.0f ),
	lastAvg( 0.0f ),
	frameCount( 0 )
	{}
	void StartFrame()
	{		
		timer.StartWatch();
	}
	void StopFrame()
	{
		timer.StopWatch();
		const float frameTime = timer.GetTimeMilli();
		timeSum += frameTime;
		timeMin = min( timeMin,frameTime );
		timeMax = max( timeMax,frameTime );
		frameCount++;
		if( frameCount >= nFramesAvg )
		{
			lastAvg = timeSum / (float)nFramesAvg;
			lastMin = timeMin;
			lastMax = timeMax;
			timeSum = 0.0f;
			timeMin = FLT_MAX;
			timeMax = 0.0f;
			frameCount = 0;
		}
	}
	float GetAvg() const
	{
		return lastAvg;
	}
	float GetMin() const
	{
		return lastMin;
	}
	float GetMax() const
	{
		return lastMax;
	}

private:
	static const int nFramesAvg = 20;
	Timer timer;
	float timeSum;
	float timeMin;
	float timeMax;
	int frameCount;
	float lastMin;
	float lastMax;
	float lastAvg;
};