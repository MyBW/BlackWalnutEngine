#include "BWDataStream.h"

#define MAX_CHAR_NUM  128

//该函数只有在binary模式下才能正常工作 否则将出现错误
std::string BWDataStream::GetLine()
{
	char buf[MAX_CHAR_NUM] ;
	std::string readString;
	int size;
	while ((size = Read(buf , MAX_CHAR_NUM - 1)) != 0)
	{
		buf[size] = '\0';
		char* p = strchr(buf, '\n');
		if (p)
		{
			Skip(p + 1 - buf - size);
			*(p) = '\0';
			readString += buf;
			if (readString.length() && readString[readString.length() - 1] =='\r')
			{
				readString.erase(readString.length() - 1, 1);
			}
			break;
		}
		readString += buf;
	}
	return readString;
}

int BWDataStream::SkipLine(const std::string& delim)
{
	char buf[MAX_CHAR_NUM];
	int total = 0;
	int readcount;
	while ((readcount = Read(buf , MAX_CHAR_NUM -1)) != 0)
	{
		buf[readcount] = '\0';
		int pos = strcspn(buf, delim.c_str());
		if (pos < readcount)
		{
			Skip(pos + 1 - readcount);
			total += pos + 1;
			break;
		}
		total += readcount;
	}
	return total;
}
void BWDataStream::GetAsString(std::string &final)
{
	char* buf = new char[size];
	Seek(0);
	Read(buf, size);
	buf[size] = '\0';
	final = buf;
	delete buf;
	Seek(0);
}
std::string BWDataStream::GetAsString()
{
	char *buf = new char[size + 1];
	Seek(0);
	int c = Read(buf, size);
	buf[c] = '\0';
	std::string final = buf;
	delete [] buf;
	Seek(0);
	return final;
}

