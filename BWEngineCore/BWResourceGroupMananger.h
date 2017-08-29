#ifndef RESOURCEGROUPMANAGER_H_
#define RESOURCEGROUPMANAGER_H_

#include "BWSingleton.h"
#include "BWResourceGroup.h"
#include "BWResourceManager.h"
#include "BWResource.h"
#include "BWCommon.h"
class loadCollision
{
public:
	void Requeset(BWResource*, BWResourceManager*){ }
};
enum ScriptLoadOrder // 脚本载入的优先级
{
	SLO_1 = 10 ,
	SLO_2 = 20 ,
	SLO_3 = 30
};
class BWResourceGroupManager : public BWSingleton<BWResourceGroupManager>
{
	friend class BWResourceManager;
public:
	static BWResourceGroupManager* GetInstance()
	{
		assert(instance);
		return instance;
	}
	BWResourceGroupManager();
	~BWResourceGroupManager();
	void RegisterResourceManager(const std::string &  ,BWResourceManager*);
	void RegisterScriptLoader(BWScriptLoader* scriptLoader , int order = SLO_3);
	BWResourceGroup* CreateResourceGroup(const std::string &);
	BWResourceGroup* GetResourceGroup(const std::string& name);
	bool AddLocationToResourceGroup(const std::string& name, const std::string & type, const std::string& groupname);
	void ParseResourceGroup(const std::string & groupName);
	void DeclareResourceGroup(const std::string & groupName , const std::string &type , const std::string &filename);
	void CreateDeclareResource(const std::string &groupName);
	void LoadResource(const std::string &groupName);
	BWDataStreamPrt OpenResource(const std::string &name, const std::string &groupName = DEFAULT_RESOURCE_GROUP);
	bool IsResourceManagerInGlobalPool(const std::string& groupName);
	loadCollision* GetLoadingCollision(){ return NULL; }
	void NotifyResourceCreated(BWResourcePtr&);
private:
	typedef std::map<std::string, BWResourceManager*> ResourceManagerMap;
	typedef std::map<std::string, BWResourceGroup*> ResourceGroupMap;
	typedef std::map<std::string, BWResourcePtr> ResourceMap; // 使用这种方式存储资源会不会造成冲突？
	typedef std::multimap<int, BWScriptLoader*> ScriptLoaderMap;  // 用int是载入的优先级 int值越大 优先级越高 相同优先级之间不能确定顺序  例如program资源要先于material资源载入 这样在material中才能设置program中的变量

	ScriptLoaderMap mScriptLoadMap;
	ResourceGroupMap resourceGroupMap;
	ResourceManagerMap resourceManagerMap;
	ResourceMap resourceMap;

	loadCollision collision; // 冲突处理

};

#endif