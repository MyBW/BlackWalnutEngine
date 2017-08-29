#ifndef	FILESYSYTEMARCHIVE_H_
#define	FILESYSYTEMARCHIVE_H_
#include "BWArchive.h"
#include "BWSmartPointer.h"
class BWFileSystemArchive : public BWArchive
{
public:
	BWFileSystemArchive();
	~BWFileSystemArchive();
	//这里的patter必须是*.xx
	FileInforVecPrt Find(const std::string &pattern);
	const BWDataStreamPrt Open(const std::string &filename , bool readOnly = true);
	bool IsExist(const std::string &name);
	bool Is_Reserved_Dir(const char *fn)  //判断是否为下一级目录
	{
		return (fn[0] == '.' && (fn[1] == 0 || (fn[1] == '.' && fn[2] == 0)));
	}
private:

};
typedef SmartPointer<BWFileSystemArchive> BWFileSystemArchivePtr;

#endif