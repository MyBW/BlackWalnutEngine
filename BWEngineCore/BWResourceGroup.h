#ifndef RESOURCEGROUP_H_
#define RESOURCEGROUP_H_

#include <list>
#include <map>
#include "AllSmartPointRef.h"
#include "BWSmartPointer.h"
#include "BWResource.h"
#include "BWArchive.h"
enum ResourceGroupStatus
{
   UNINITALSED
};
class BWResourceGroup
{
	friend class BWResourceGroupManager;
public:
	BWResourceGroup();
	~BWResourceGroup();
	bool AddLocation(const std::string& name , const std::string& type);
	void AddCreatedResource(const BWResourcePtr &resource);
private:

	struct Location 
	{
		BWArchive* location;
		bool retrive;
	};
	struct  DeclareResource
	{
		std::string resourceName; //资源名字
		std::string resourceType;//资源类型 用来确定resourceMananger
		//如何确定资源路径
	};
	typedef std::list<Location*> LoactioinList;
	typedef std::map<std::string, BWArchive*> ResourceLoactionIndex; //不会引起冲突，因为文件名包含了后缀 如果是两个一样的文件名 说明资源重合 需要更改资源放置方式
	typedef std::list<DeclareResource*> DeclearResourceList;
	typedef std::list<BWResourcePtr> LoadedUnloadReourceList;
	std::string name;
	ResourceGroupStatus status;
	LoactioinList locationList;
	ResourceLoactionIndex resourceLocationIndex;
	DeclearResourceList declearResourceList;	
	LoadedUnloadReourceList loadedunloadResourceList;
};

#endif