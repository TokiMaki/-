#pragma once
//-----------------------------------------------------------------------------
// File: CGameTimer.h
//-----------------------------------------------------------------------------
#define MAX_SAMPLE_COUNT 60 // Maximum frame time sample count

class CGameTimer
{
public:
	CGameTimer();
	virtual ~CGameTimer();

	void Tick(float fLockFPS = 0.0f);
	void Start();
	void Stop();
	void Reset();

    // unsigned long GetFrameRate(LPTSTR lpszString, int nCharacters = 0);
    float GetTimeElapsed();
	float GetTotalTime();

private:
	double							m_fTimeScale;						
	float							m_fTimeElapsed;		

	__int64							m_nBasePerformanceCounter;
	__int64							m_nPausedPerformanceCounter;
	__int64							m_nStopPerformanceCounter;
	__int64							m_nCurrentPerformanceCounter;
    __int64							m_nLastPerformanceCounter;

	__int64							m_nPerformanceFrequencyPerSec;				

    float							m_fFrameTime[MAX_SAMPLE_COUNT];
    unsigned long					m_nSampleCount;

    unsigned long					m_nCurrentFrameRate;				
	unsigned long					m_nFramesPerSecond;					
	float							m_fFPSTimeElapsed;		

	bool							m_bStopped;
};
