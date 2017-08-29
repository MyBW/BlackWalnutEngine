#include "BWFileStreamDataStream.h"

BWFileStreamDataStream::BWFileStreamDataStream(std::ifstream*s, bool freeOnClose /* = true */) :BWDataStream(), iStream(s), fileReadOnlyStream(s), fileStream(0), freeOnClose(freeOnClose)
{
	iStream->seekg(0, std::ios_base::end);
	size = (int)iStream->tellg();
	iStream->seekg(0, std::ios_base::beg);
	DetermineAccess();
}
BWFileStreamDataStream::BWFileStreamDataStream(std::fstream * s, bool _freeOnClose /* = true */) :BWDataStream(), iStream(s), fileReadOnlyStream(0), fileStream(s), freeOnClose(_freeOnClose)
{
	iStream->seekg(0, std::ios_base::end);
	size = (int)iStream->tellg();
	iStream->seekg(0, std::ios_base::beg);
	DetermineAccess();
}
BWFileStreamDataStream::BWFileStreamDataStream(const std::string & name, std::fstream * s, bool _freeOnClose /* = true */) :BWDataStream(name), iStream(s), fileReadOnlyStream(0), fileStream(s), freeOnClose(_freeOnClose)
{
	iStream->seekg(0, std::ios_base::end);
	size = (int)iStream->tellg();
	iStream->seekg(0, std::ios_base::beg);
	DetermineAccess();
	this->name = name;
}
BWFileStreamDataStream::BWFileStreamDataStream(const std::string & name, std::ifstream * s, bool _freeOnClose /* = true */) :BWDataStream(name), iStream(s), fileReadOnlyStream(s), fileStream(0), freeOnClose(_freeOnClose)
{
	iStream->seekg(0, std::ios_base::end);
	size = (int)iStream->tellg();
	iStream->seekg(0, std::ios_base::beg);
	DetermineAccess();
	this->name = name;
}
BWFileStreamDataStream::BWFileStreamDataStream(const std::string &fileName, bool readOnly) :BWDataStream(fileName), iStream(NULL), fileStream(NULL), fileReadOnlyStream(NULL), freeOnClose(true)
{
	
	if (readOnly)
	{
		std::ifstream *filestream = new std::ifstream;
		filestream->open(fileName.c_str(), std::ios_base::in | std::ios_base::binary);
		iStream = filestream;
		fileReadOnlyStream = filestream;
		if (!filestream->is_open())
		{
			//抛出异常  配置文件出错
			return;
		}
		iStream->seekg(0, std::ios_base::end);
		size = (int)iStream->tellg();
		iStream->seekg(0, std::ios_base::beg);
		
	}
	else
	{
		std::fstream * fileStream = new std::fstream;
		fileStream->open(fileName.c_str(), std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		this->fileStream = fileStream;
		if (!fileStream->is_open())
		{
			//抛出异常  配置文件出错
			return;
		}
		fileStream->seekg(0, std::ios_base::end);
		size = (int)fileStream->tellg();
		fileStream->seekg(0, std::ios_base::beg);
	}
	DetermineAccess();
	this->name = name;
}
BWFileStreamDataStream::~BWFileStreamDataStream()
{
	Close();
}
void BWFileStreamDataStream::DetermineAccess()
{
	accessMode = 0;
	if (iStream)
	{
		accessMode |= READ;
	}
	if (fileStream)
	{
		accessMode |= WRITE;
	}
}
int BWFileStreamDataStream::Read(void* buf, int count)
{
	iStream->read(static_cast<char *>(buf), static_cast<std::streamsize>(count));
	return iStream->gcount();
}
int BWFileStreamDataStream::Write(const void* buf, int size)
{
	int count = 0;
	if (IsWriteable() && fileStream)
	{
		fileStream->write(static_cast<const char*>(buf), static_cast<std::streamsize>(size));
		count = size;
	}
	return count;
}
int BWFileStreamDataStream::ReadLine(char *buf, int maxCount, const std::string& deadchars)
{
	if (deadchars.empty())
	{
		//抛出异常
		return -1;
	}
	bool trimCR = false;
	if (deadchars.at(0) == '\n')
	{
		trimCR = true;
	}
	iStream->getline(static_cast<char*>(buf), static_cast<std::streamsize>(maxCount + 1), deadchars.at(0));
	int size = iStream->gcount();
	if (iStream->eof())
	{
	}
	else if (iStream->fail())
	{
		if (size == maxCount)
		{
			iStream->clear();
		}
		else
		{
			//抛出异常 读取失败
			return -1;
		}
	}
	else
	{
		--size;
	}
	if (trimCR && buf[size - 1] == '\r')
	{
		--size;
		buf[size] = '\0';
	}
	return size;
}
bool BWFileStreamDataStream::Eof()
{
	return iStream->eof();
}
int BWFileStreamDataStream::Tell()
{
	iStream->clear();
	return iStream->tellg();
}
void BWFileStreamDataStream::Seek(int pos)
{
	iStream->clear();
	iStream->seekg(static_cast<std::streamoff>(pos) , std::ios_base::beg);
}

void BWFileStreamDataStream::Skip(int count)
{
	iStream->clear();
	iStream->seekg(static_cast<std::ifstream::pos_type>(count), std::ios_base::cur);
}

void BWFileStreamDataStream::Close()
{
	if (iStream)
	{
		if (fileReadOnlyStream)
		{
			fileReadOnlyStream->close();
		}
		if (fileStream)
		{
			fileStream->flush();
			fileStream->close();
		}
	}
	if (freeOnClose)
	{
		if (fileReadOnlyStream)
		{
			delete fileReadOnlyStream;
		}
		if (fileStream)
		{
			delete fileStream;
		}
		fileStream = NULL;
		fileReadOnlyStream = NULL;
		iStream = NULL;
	}
}