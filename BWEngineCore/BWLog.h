#ifndef LOG_H_
#define LOG_H_
#include <fstream>
#include "BWSingleton.h"

class Log : public BWSingleton<Log>
{
public:
	static Log* GetInstance();
	Log(bool isOutToFile = true , const std::string &fileName ="debugLog.txt");
	void logMessage(const std::string &message);
	void logMessage(const std::string &message, bool outToFile);
	void setOutToFile(bool isOutToFile);
private:
	std::ofstream mOutStream;
	bool mIsDebugModel;
	bool mIsOutToFile;
	std::string mFileName;
	bool mIsOutTime;
};

#endif