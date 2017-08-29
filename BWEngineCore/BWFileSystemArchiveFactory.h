#ifndef FILESYSTEMARCHIVEFACTORY_H_
#define FILESYSTEMARCHIVEFACTORY_H_
#include "BWArchiveFactory.h"
class BWFileSystemArchiveFactory : public BWArchiveFactory
{
public:
	BWFileSystemArchiveFactory();
	~BWFileSystemArchiveFactory();
	virtual BWArchive* GetArchive();
};


#endif