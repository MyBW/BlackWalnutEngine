#ifndef FILESTREAMDATASTREAM_H_
#define FILESTREAMDATASTREAM_H_

#include "BWDataStream.h"
#include "BWSingleton.h"
#include <fstream>
class BWFileStreamDataStream : public BWDataStream 
{
public:
	//freeOnClose的作用是 如果是在外面new的stream 那么要不要该类释放掉
	BWFileStreamDataStream(std::ifstream*s, bool freeOnClose = true);
	BWFileStreamDataStream(std::fstream*s, bool freeOnClose = true);
	BWFileStreamDataStream(const std::string & fileName , bool readOnly = true);
	BWFileStreamDataStream(const std::string & filename, std::ifstream*s, bool freeOnClose = true);
	BWFileStreamDataStream(const std::string & filename, std::fstream * s, bool freeOnClose = true);
	~BWFileStreamDataStream();
	int Read(void* buf, int count);
	int Write(const void* buf, int size);
	int ReadLine(char *buf, int maxCount, const std::string& deadchars);
	bool Eof();
	int Tell();
	void Seek(int pos);
	void Skip(int count);
	void Close();
private:
	void DetermineAccess();
	std::istream* iStream;
	std::ifstream* fileReadOnlyStream;
	std::fstream* fileStream;
	bool freeOnClose;
};



#endif