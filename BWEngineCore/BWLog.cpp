#include "BWLog.h"
#include <cassert>
#include <iostream>
#include <time.h>
Log::Log(bool isOutToFile /* = true  */, const std::string &fileName /* ="debugLog.txt" */) :
mIsOutToFile(isOutToFile), 
mFileName(fileName),
mIsDebugModel(false),
mIsOutTime(false)
{
     if (isOutToFile)
     {
		 mOutStream.open(fileName.c_str(), std::ios_base::out | std::ios_base::binary);
		 if (!mOutStream.is_open())
		 {
			 assert(0);
		 }
     }
}
Log*  Log::GetInstance()
{
	assert(instance);
	return instance;
}
void Log::setOutToFile(bool isOutToFile)
{
	mIsOutToFile = isOutToFile;
	if (mIsOutToFile && !mOutStream.is_open())
	{
		mOutStream.open(mFileName.c_str(), std::ios_base::out | std::ios_base::binary);
		if (!mOutStream.is_open())
		{
			assert(0);
		}
	}
}
void Log::logMessage(const std::string &message, bool outToFile)
{
	if (outToFile)
	{
		mOutStream << message.c_str() << std::endl;
	}
	else
	{
		std::cout << message.c_str() << std::endl;
	}
}
void Log::logMessage(const std::string &message)
{
	if (mIsOutToFile)
	{
		if (mIsOutTime)
		{
			tm *ptime;
			time_t ctTime;
			time(&ctTime);
			ptime = localtime(&ctTime);
			mOutStream << ptime->tm_hour << " : "<< ptime->tm_min << " : "<< ptime->tm_sec << "    ";
		}
		mOutStream << message.c_str() << std::endl;
	}
	else
	{
		std::cout << message.c_str() << std::endl;
	}
}







