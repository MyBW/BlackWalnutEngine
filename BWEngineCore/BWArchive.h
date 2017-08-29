#ifndef ARCHIVE_H_
#define ARCHIVE_H_

#include <string>
#include <vector>
#include "BWSmartPointer.h"
#include "BWDataStream.h"

class BWArchive;
struct FileInfo
{
	BWArchive*  archive;
	std::string fileName;
	std::string path;
	std::string baseName;
	int compressedSize;
	int uncompressedSize;
};
typedef std::vector<FileInfo> FileInforVec;
typedef SmartPointer<FileInforVec> FileInforVecPrt;
class BWArchive
{
public:
	BWArchive(const std::string &ty);
	virtual ~BWArchive();
	//找到该路径下所有包含该pattern后缀的文件的信息
	virtual FileInforVecPrt Find(const std::string &pattern) = 0;
	virtual const BWDataStreamPrt  Open(const std::string &filename , bool readOnly = true) = 0;
	const std::string &GetType(){ return type; }
	const void SetName(const std::string &name){ this->name = name; }
	 virtual bool IsExist(const std::string &name) =0;
protected:
	std::string name;
	std::string type;
};
typedef SmartPointer<BWArchive> BWArchivePtr;

#endif