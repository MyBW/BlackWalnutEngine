#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include <string>
#include <map>
#include "BWSmartPointer.h"
#include "BWScriptLoader.h"
#include "BWResourceGroupMananger.h"
#include "BWCommon.h"
#include "BWResource.h"

class BWResourceManager : public BWScriptLoader
{
public:
	typedef std::pair<BWResourcePtr, bool> ResourceCreateOrRetrieveResult;
public:
	BWResourceManager();
	~BWResourceManager();
	virtual BWResourcePtr Create(const std::string &name, const std::string &groupName);
	virtual BWResourcePtr Create(const std::string &name, const std::string &gropName,const NameValuePairList *param);
	void ParseScript(const BWDataStream& dataStream);
	BWResourcePtr GetResource(const std::string &name , const std::string &groupName = DEFAULT_RESOURCE_GROUP);
	 void NotifyResourceLoaded(BWResourcePtr resource);
	virtual ResourceCreateOrRetrieveResult CreateOrRetrieve(const std::string &name, const std::string &groupName,const NameValuePairList* nameValue = NULL);
protected:
	//模板方法
	virtual BWResourcePtr CreateImp(const std::string &name , const std::string &groupName);
	virtual void AddImp(BWResourcePtr resource);
protected:
	typedef std::map<std::string, BWResourcePtr> ResourceMap;
	typedef std::map<std::string, ResourceMap> ResourceWithGroup;
	typedef std::map<int, BWResourcePtr> ResouerceHandleMap;
	
	//这里存在两种创建自愿的方式 一种是将资源放入 resourcegroupmanager的资源组中 这是通过resourcegroupmanager创建的资源
	//另一中是通过resourcemanager 创建的资源  例如在应用运行时  创建资源  此时资源组并不一定在resourcegroupmanager中
	//这时 就是要把资源放入 resourcewithgroup中  
	ResourceMap resourceMap; //这个map和resourcegroupmanager的资源最中的数据保持一致
	ResourceWithGroup  resourceWithGroup;//自己的资源组
	ResouerceHandleMap resourceHnadleMap;// 通过索引来查找资源

	std::string resourceType;
};

#endif