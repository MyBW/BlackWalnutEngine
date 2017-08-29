#ifndef DATASTREAM_H_
#define DATASTREAM_H_
//这个类是对大块数据操作的一个抽象  大块数据可能是从文件中读取的 也可能是游戏中产生的 
#include <string>
#include "BWSmartPointer.h"
class BWDataStream
{
public:
	enum AccessMode
	{
		READ = 1,
		WRITE = 2
	};
	BWDataStream(int _accessMode = READ) :accessMode(_accessMode), size(0){}
	BWDataStream(const std::string &_name, int _accessMode = READ) :accessMode(_accessMode), name(_name){ }
	virtual ~BWDataStream(){}
	bool IsNull() const;
	const std::string& GetName();
	unsigned short GetAccessModel(){return accessMode; }
	bool IsReadable(){ return (accessMode&READ) != 0; }
	bool IsWriteable(){ return (accessMode&WRITE) != 0; }
	int Write(const void* buf, int size);
	virtual int Read(void*buf, int size) = 0;
	//只有在用二进制模式打开的情况下才能正确的工作
	virtual int ReadLine(char *buf, int maxCount, const std::string& deadchars) = 0;
	//只有在使用二进制模式打开的状态下才能正常工作
	std::string GetLine();
	//只有在使用二进制模式打开的状态下才能正常工作
	virtual int SkipLine(const std::string& delim = "\n");
	std::string GetAsString();
	void GetAsString(std::string &final);
	virtual void Skip(int count) =0;
	virtual void Seek(int pos) = 0;
	virtual int  Tell() = 0;
	virtual bool Eof() = 0;
	virtual void Close() = 0;
	int Size() {return size;}
protected:
	std::string name;
	int size;
	unsigned short accessMode;
};
typedef SmartPointer<BWDataStream> BWDataStreamPrt;

#endif