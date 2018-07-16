#ifndef _CTIMER_H_
#define _CTIMER_H_

// Definitions

// Library Includes:

// Local Includes:

// Class Declaration:
class CTimer
{
	// class functions
	public:
		CTimer();
		~CTimer();

		float GetGameTime() const;
		float GetDeltaTime() const; 

		void Reset();
		void Start();
		void Stop(); 
		void Tick(); 

	protected:

	private:

	// class members
	public:

	protected:

	private:
		double m_dSecondsPerCount, m_dDeltaTime;
		long long m_llBaseTime, m_llPausedTime, m_llStopTime, m_llPreviousTime, m_llCurrentTime;

		bool m_bStopped;
};

#endif // _CTIMER_H_