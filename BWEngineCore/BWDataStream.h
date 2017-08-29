#ifndef DATASTREAM_H_
#define DATASTREAM_H_
//������ǶԴ�����ݲ�����һ������  ������ݿ����Ǵ��ļ��ж�ȡ�� Ҳ��������Ϸ�в����� 
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
	//ֻ�����ö�����ģʽ�򿪵�����²�����ȷ�Ĺ���
	virtual int ReadLine(char *buf, int maxCount, const std::string& deadchars) = 0;
	//ֻ����ʹ�ö�����ģʽ�򿪵�״̬�²�����������
	std::string GetLine();
	//ֻ����ʹ�ö�����ģʽ�򿪵�״̬�²�����������
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