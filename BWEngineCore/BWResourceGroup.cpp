#include "BWResourceGroup.h"
#include "BWArchiveManager.h"
BWResourceGroup::BWResourceGroup():status(UNINITALSED)
{

}

bool BWResourceGroup::AddLocation(const std::string& name, const std::string& type)
{
	BWArchive* archive = BWArchiveManager::GetInstance()->GetArchive(name, type);
	if (archive == NULL)
	{
		return false;
	}
	Location* loc = new Location();
	loc->location = archive;
	locationList.push_back(loc);
	FileInforVecPrt filename = archive->Find("*");
	FileInforVec::iterator itor = filename.Get()->begin();
	while (itor != filename.Get()->end())
	{
		resourceLocationIndex[(itor->baseName)] = archive;
		itor++;
	}
	return true;
}

void BWResourceGroup::AddCreatedResource(const BWResourcePtr& resource)
{
	loadedunloadResourceList.push_back(resource);
}