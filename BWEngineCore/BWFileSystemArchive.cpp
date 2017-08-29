#include "BWFileSystemArchive.h"
#include "BWFileStreamDataStream.h"
#include <io.h>
#include <sys/stat.h>
BWFileSystemArchive::BWFileSystemArchive() :BWArchive("FileSystem")
{

}
BWFileSystemArchive::~BWFileSystemArchive()
{

}
const BWDataStreamPrt  BWFileSystemArchive::Open(const std::string &filename , bool readOnly)
{
	std::string path = name + '/' + filename;
	BWFileStreamDataStream* dataStream = new BWFileStreamDataStream(path, readOnly);
	return dataStream;
}
bool BWFileSystemArchive::IsExist(const std::string &name)
{
	std::string fullPath = this->name + '/' + name;
	struct stat tagStat;
	return (stat(fullPath.c_str(), &tagStat) == 0);
}
FileInforVecPrt BWFileSystemArchive::Find(const std::string &pattern)
{
	long handle, res;
	_finddata_t tagData;
	std::string full_path = name + '/' + pattern;
	handle = _findfirst(full_path.c_str(), &tagData);
	res = 0;
	FileInforVec *fileInforVec = new FileInforVec;
	while (handle != -1 && res != -1)
	{
		if ((tagData.attrib & _A_HIDDEN) == 0 && !Is_Reserved_Dir(tagData.name) && (tagData.attrib & _A_SUBDIR) == 0)
		{
			FileInfo fileInfor;
			fileInfor.archive = this;
			fileInfor.baseName = tagData.name;
			fileInfor.fileName = name + '/' + tagData.name;
			fileInfor.path = name;
			fileInfor.compressedSize = tagData.size;
			fileInfor.uncompressedSize = tagData.size;
			fileInforVec->push_back(fileInfor);
		}
		res = _findnext(handle, &tagData);
	}
	if (handle != -1)
	{
		_findclose(handle);
	}
	return fileInforVec;
}