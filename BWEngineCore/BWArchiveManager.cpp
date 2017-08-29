#include "BWArchiveManager.h"


BWArchiveManager* BWArchiveManager::GetInstance()
{
	assert(instance);
	return instance;
}
BWArchiveManager::BWArchiveManager()
{

}
BWArchiveManager::~BWArchiveManager()
{
	FactoryMap::iterator itor = factoryMap.begin();
	while (itor != factoryMap.end())
	{
		delete(itor->second);
		itor->second = NULL;
		itor++;
	}
	factoryMap.clear();
	archiveMap.clear();
}

BWArchive* BWArchiveManager::GetArchive(const std::string name, const std::string type)
{
	ArchiveMap::iterator result = archiveMap.find(name);
	if (result != archiveMap.end())
	{
		int num = archiveMap.count(name);
		while (num)
		{
			if (result->second->GetType() == type)
			{
				return result->second.Get();
			}
			result++;
			num--;
		}
	}
	FactoryMap::iterator factory = factoryMap.find(type);
	if (factoryMap.end() != factory)
	{
		BWArchivePtr archive = factory->second->GetArchive();
		archive->SetName(name);
		archiveMap.insert(ArchiveMap::value_type(name, archive));
		return archive.Get();
	}
	//抛出异常
	return NULL;
}
void BWArchiveManager::RegisterFactory(BWArchiveFactory* factory)
{
	if (factoryMap.find(factory->GetType()) != factoryMap.end())
	{
		//抛出异常
		return;
	}
	factoryMap.insert(FactoryMap::value_type(factory->GetType(), factory));
}