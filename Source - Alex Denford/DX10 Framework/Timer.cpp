//=======================================================================================
// GameTimer.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Timer.h"
#include <windows.h>

CTimer::CTimer() : m_dSecondsPerCount(0.0), m_dDeltaTime(-1.0), m_llBaseTime(0), 
				m_llPausedTime(0), m_llPreviousTime(0), m_llCurrentTime(0), m_bStopped(false)
{
	long long llCountsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&llCountsPerSec);
	m_dSecondsPerCount = 1.0 / (double)llCountsPerSec;
}

CTimer::~CTimer()
{

}

// Returns the total time elapsed since reset() was called, NOT counting any
// time when the clock is stopped.
float CTimer::GetGameTime() const
{
	// If we are stopped, do not count the time that has passed since we stopped.
	//
	// ----*---------------*------------------------------*------> time
	//  mBaseTime       mStopTime                      mCurrentTime

	if(m_bStopped)
	{
		return((float)((m_llStopTime - m_llBaseTime) * m_dSecondsPerCount));
	}

	// The distance mCurrentTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrentTime:  
	//
	//  (mCurrentTime - mPausedTime) - mBaseTime 
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrentTime
	
	else
	{
		return((float)(((m_llCurrentTime - m_llPausedTime) - m_llBaseTime) * m_dSecondsPerCount));
	}
}

float CTimer::GetDeltaTime() const
{
	return((float)m_dDeltaTime);
}

void CTimer::Reset()
{
	long long llCurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&llCurrTime);

	m_llBaseTime = llCurrTime;
	m_llPreviousTime = llCurrTime;
	m_llStopTime = 0;
	m_bStopped  = false;
}

void CTimer::Start()
{
	long long llStartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&llStartTime);


	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if(m_bStopped)
	{
		m_llPausedTime += (llStartTime - m_llStopTime);	

		m_llPreviousTime = llStartTime;
		m_llStopTime = 0;
		m_bStopped  = false;
	}
}

void CTimer::Stop()
{
	if(!m_bStopped)
	{
		long long llCurrTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&llCurrTime);

		m_llStopTime = llCurrTime;
		m_bStopped  = true;
	}
}

void CTimer::Tick()
{
	if(m_bStopped)
	{
		m_dDeltaTime = 0.0;
		return;
	}

	long long llCurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&llCurrTime);
	m_llCurrentTime = llCurrTime;

	// Time difference between this frame and the previous.
	m_dDeltaTime = (m_llCurrentTime - m_llPreviousTime) * m_dSecondsPerCount;

	// Prepare for next frame.
	m_llPreviousTime = m_llCurrentTime;

	// Force nonnegative.  The DXSDK's CDXUTCTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if(m_dDeltaTime < 0.0)
	{
		m_dDeltaTime = 0.0;
	}
}