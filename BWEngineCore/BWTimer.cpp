#include "BWRoot.h"

unsigned long BWTimer::getMilliseconds()
{
	return 100;
	/*LARGE_INTEGER curTime;
	HANDLE thread = GetCurrentThread();
	DWORD_PTR oldMask = SetThreadAffinityMask(thread, mTimerMask);

	QueryPerformanceCounter(&curTime);
	SetThreadAffinityMask(thread, oldMask);
	LONGLONG newtime = curTime.QuadPart - mStartTime.QuadPart;
	unsigned long newTick = (unsigned long)(1000 * newtime / mFrequency.QuadPart);*/
}