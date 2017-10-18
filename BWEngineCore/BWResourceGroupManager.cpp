#include "BWResourceGroupMananger.h"

BWResourceGroupManager::BWResourceGroupManager()
{

}

BWResourceGroupManager::~BWResourceGroupManager()
{

}

BWResourceGroup* BWResourceGroupManager::CreateResourceGroup(const std::string& name)
{
	if (GetResourceGroup(name))
	{
		//这里要有异常 ， 比较严重的错误
		return GetResourceGroup(name);
	}
	BWResourceGroup* resourceGroup = new BWResourceGroup();
	resourceGroup->name = name;
	resourceGroupMap.insert(ResourceGroupMap::value_type(name, resourceGroup));
	return resourceGroup;
}
bool BWResourceGroupManager::IsResourceManagerInGlobalPool(const std::string& groupName)
{
	BWResourceGroup* group = GetResourceGroup(groupName);
	if (group)
	{
		return true;
	}
	return false;
}
void  BWResourceGroupManager::NotifyResourceCreated(BWResourcePtr& resource )
{
	BWResourceGroup* group = GetResourceGroup(resource->GetGroupName());
	if (!group)
	{
		return;
	}
	group->AddCreatedResource(resource);
}
BWResourceGroup* BWResourceGroupManager::GetResourceGroup(const std::string& name)
{
	ResourceGroupMap::iterator  reslut = resourceGroupMap.find(name);
	if (reslut == resourceGroupMap.end())
	{
		return NULL;
	}
	return reslut->second;
}
bool  BWResourceGroupManager::AddLocationToResourceGroup(const std::string& localname, const std::string & type, const std::string & groupname)
{
	BWResourceGroup* group = GetResourceGroup(groupname);
	if (!group)
	{
		group = CreateResourceGroup(groupname);
	}
	return group->AddLocation(localname, type);
}
void BWResourceGroupManager::CreateDeclareResource(const std::string &groupName)
{
	BWResourceGroup* group = GetResourceGroup(groupName);
	if (!group)
	{
		//这里要抛出异常
		return;
	}
	BWResourceGroup::DeclearResourceList& declareResourceList = group->declearResourceList;
	BWResourceGroup::DeclearResourceList::iterator itor = declareResourceList.begin();
	while (itor != declareResourceList.end())
	{
		BWResourceManager* mgr = resourceManagerMap.find((*itor)->resourceType)->second;
		BWResourcePtr resource = mgr->Create((*itor)->resourceName, groupName);
		group->loadedunloadResourceList.push_back(resource);
		 resourceMap[resource.Get()->GetResourceName()] = resource;
		itor++;
	}
}
void BWResourceGroupManager::DeclareResourceGroup(const std::string & groupName, const std::string &type, const std::string &filename)
{
	BWResourceGroup* group = GetResourceGroup(groupName);
	if (!group)
	{
		//这里要抛出异常
		return;
	}

	BWResourceGroup::DeclareResource *del =  new BWResourceGroup::DeclareResource;
	del->resourceName = filename;
	del->resourceType = type;
	group->declearResourceList.push_back(del);
}
void BWResourceGroupManager::ParseResourceGroup(const std::string &groupName)
{
	BWResourceGroup* group = GetResourceGroup(groupName);
	if (!group)
	{
		//这里要抛出异常
		return;
	}
	typedef SmartPointer<BWResourceGroup::ResourceLoactionIndex> SmartFileList;
	SmartFileList filelist ;
	std::map<BWScriptLoader*, SmartFileList> parseFileList;
	BWResourceGroup::ResourceLoactionIndex& resourceLoactionIndex = group->resourceLocationIndex;
	BWResourceGroup::ResourceLoactionIndex::iterator itor ;
	/*ScriptLoaderVec::iterator  scriptLoader = scriptLoaderVec.begin();

	while (scriptLoader != scriptLoaderVec.end())
	{
		const std::string &pattern = (*scriptLoader)->GetPattern();
			itor = resourceLoactionIndex.begin();
			filelist = new BWResourceGroup::ResourceLoactionIndex;
			while (itor != resourceLoactionIndex.end())
			{
				std::string resourcePattern = itor->first;
				resourcePattern = resourcePattern.substr(resourcePattern.find_last_of("."), resourcePattern.length() - resourcePattern.find_last_of("."));
				if (pattern == resourcePattern)
				{
					(*filelist.Get())[itor->first] = itor->second;
				}
				itor++;
			}
			parseFileList[*scriptLoader] = filelist;

		scriptLoader++;
	}*/
	ScriptLoaderMap::iterator scriptLoader = mScriptLoadMap.begin();
	while (scriptLoader != mScriptLoadMap.end())
	{
		const std::string &pattern = scriptLoader->second->GetPattern();
		itor = resourceLoactionIndex.begin();
		filelist = new BWResourceGroup::ResourceLoactionIndex;
		while (itor != resourceLoactionIndex.end())
		{
			std::string resourcePattern = itor->first;
			resourcePattern = resourcePattern.substr(resourcePattern.find_last_of("."), resourcePattern.length() - resourcePattern.find_last_of("."));
			if (pattern == resourcePattern)
			{
				(*filelist.Get())[itor->first] = itor->second;
			}
			itor++;
		}
		parseFileList[scriptLoader->second] = filelist;

		scriptLoader++;
	}
	scriptLoader = mScriptLoadMap.begin();
	while (scriptLoader != mScriptLoadMap.end())
	{
		std::map<BWScriptLoader*, SmartFileList>::iterator scriptloaderfilelist = parseFileList.begin();
		while (scriptloaderfilelist != parseFileList.end())
		{
			BWScriptLoader* scriptloader = scriptloaderfilelist->first;
			if (scriptloader->order == scriptLoader->first)
			{
				BWResourceGroup::ResourceLoactionIndex::iterator filelist = scriptloaderfilelist->second.Get()->begin();
				while (filelist != scriptloaderfilelist->second.Get()->end())
				{
					BWArchive* archive = filelist->second;
					const BWDataStreamPrt datastream = archive->Open(filelist->first);
					if (!datastream.IsNull())
					{
						scriptloader->ParseScript(*(datastream.Get()), groupName);
					}
					filelist++;
				}
				break;
			}
			scriptloaderfilelist++;
		}
		scriptLoader++;
	}
	
} 

void BWResourceGroupManager::LoadResource(const std::string &groupName)
{
	BWResourceGroup* group = GetResourceGroup(groupName);
	if (!group)
	{
		//抛出异常
		return;
	}
	BWResourceGroup::LoadedUnloadReourceList& resourceList = group->loadedunloadResourceList;
	BWResourceGroup::LoadedUnloadReourceList::iterator itor = resourceList.begin();
	while(itor != resourceList.end())
	{
		itor->Get()->Load();
		itor++;
	}
}

void BWResourceGroupManager::RegisterResourceManager(const std::string & pattern, BWResourceManager* manager)
{
	std::pair<ResourceManagerMap::iterator , bool> result = resourceManagerMap.insert(ResourceManagerMap::value_type(pattern, manager));
	if (result.second == false)
	{
		//异常
	}
}
void BWResourceGroupManager::RegisterScriptLoader(BWScriptLoader* scriptLoader , int order)
{
	//scriptLoaderVec.push_back(scriptLoader);
	mScriptLoadMap.insert(ScriptLoaderMap::value_type(order, scriptLoader));
	
}
BWDataStreamPrt BWResourceGroupManager::OpenResource(const std::string &name, const std::string &groupName /* = DEFAULT_RESOURCE_GROUP */)
{
	BWResourceGroup* resource = GetResourceGroup(groupName);
	if (!resource)
	{
		// wrong
		return NULL;
	}
	BWResourceGroup::ResourceLoactionIndex::iterator itor = resource->resourceLocationIndex.find(name);
	if (itor != resource->resourceLocationIndex.end())
	{
		BWDataStreamPrt ret = itor->second->Open(name);
		return ret;
	}
	BWResourceGroup::LoactioinList::iterator loaction = resource->locationList.begin();
	while (loaction != resource->locationList.end())
	{
		if ((*loaction)->location->IsExist(name))
		{
			BWDataStreamPrt ret = (*loaction)->location->Open(name);
			return ret;
		}
		loaction++;
	}
	return NULL;
}