#include "BWFileSystemArchiveFactory.h"
#include "BWFileSystemArchive.h"
BWFileSystemArchiveFactory::BWFileSystemArchiveFactory() :BWArchiveFactory("FileSystem")
{

}
BWFileSystemArchiveFactory::~BWFileSystemArchiveFactory()
{

}
BWArchive* BWFileSystemArchiveFactory::GetArchive()
{
	return new BWFileSystemArchive();
}