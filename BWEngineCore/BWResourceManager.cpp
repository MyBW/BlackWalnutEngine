#include "BWResourceManager.h"
#include "BWCommon.h"
BWResourceManager::BWResourceManager()
{

}
BWResourceManager::~BWResourceManager()
{

}
BWResourcePtr BWResourceManager::Create(const std::string &name, const std::string &groupName, const NameValuePairList *param)
{
	return Create(name, groupName);
}
BWResourcePtr BWResourceManager::Create(const std::string &name, const std::string &groupName)
{
	BWResourcePtr res = CreateImp(name, groupName);
	AddImp(res);
	BWResourceGroupManager::GetInstance()->NotifyResourceCreated(res);	
	return res;
}
BWResourcePtr BWResourceManager::GetResource(const std::string &name , const std::string &groupName)
{
     if (BWResourceGroupManager::GetInstance()->IsResourceManagerInGlobalPool(groupName))
     {
		 if (resourceMap.size() == 0 )
		 {
			 return NULL;
		 }
		 ResourceMap::iterator itor = resourceMap.find(name);
		 if (itor == resourceMap.end())
		 {
			 return NULL;
		 }
		 return resourceMap.find(name)->second;
     }
	 else
	 {
		 ResourceWithGroup::iterator result = resourceWithGroup.find(groupName);
		 if (result != resourceWithGroup.end())
		 {
			 ResourceMap::iterator resource = result->second.find(name);
			 if (resource != result->second.end())
			 {
				 return (resource->second);
			 }
		 }
		 return  NULL;
	 }
}
BWResourcePtr BWResourceManager::CreateImp(const std::string &name , const std::string &groupName)
{
	return NULL;
}
void BWResourceManager::AddImp(BWResourcePtr resource)
{
	if (resource.IsNull())
	{
		return;
	}
	std::pair<ResourceMap::iterator, bool> result;
	if (BWResourceGroupManager::GetInstance()->IsResourceManagerInGlobalPool(resource->GetGroupName()))
	{
		result = resourceMap.insert(ResourceMap::value_type(resource->GetResourceName(), resource));
	}
	else
	{
		ResourceWithGroup::iterator res;
		res = resourceWithGroup.find(resource->GetGroupName());
		if (res == resourceWithGroup.end())
		{
			ResourceMap dummy;
		    resourceWithGroup.insert(ResourceWithGroup::value_type(resource->GetGroupName(), dummy));
			res = resourceWithGroup.find(resource->GetGroupName());
		}
		result = res->second.insert(ResourceMap::value_type(resource->GetResourceName(), resource));
	}
	if (!result.second)
	{
		//³åÍ»´¦Àí
		if (BWResourceGroupManager::GetInstance()->GetLoadingCollision())
		{
			BWResourceGroupManager::GetInstance()->GetLoadingCollision()->Requeset(resource.Get(), this);
			this->AddImp(resource);
		}
	}
	else
	{
		resourceHnadleMap.insert(ResouerceHandleMap::value_type(resource->GetID(), resource));
	}
}

BWResourceManager::ResourceCreateOrRetrieveResult BWResourceManager::CreateOrRetrieve(const std::string &name, const std::string &groupName, const NameValuePairList* nameValue)
{
	BWResourcePtr resourece = GetResource(name, groupName);
	bool created = false;
	if (resourece.IsNull())
	{
		created = true;
		resourece = Create(name, groupName, nameValue);
	}
	return  ResourceCreateOrRetrieveResult(resourece, created);
}