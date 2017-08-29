#ifndef ARCHIVEMANAGER_H_
#define ARCHIVEMANAGER_H_

#include <map>
#include "BWSingleton.h"
#include "BWArchiveFactory.h"
#include "BWArchive.h"
class BWArchiveManager : public BWSingleton<BWArchiveManager>
{
public:
	static BWArchiveManager *GetInstance();
	BWArchiveManager();
	~BWArchiveManager();
	BWArchive* GetArchive(const std::string name , const std::string type);
	void RegisterFactory(BWArchiveFactory*);
private:
	typedef std::map<std::string, BWArchiveFactory*> FactoryMap;
	typedef std::multimap<std::string ,BWArchivePtr> ArchiveMap;
	FactoryMap factoryMap;
	ArchiveMap archiveMap;
};


#endif