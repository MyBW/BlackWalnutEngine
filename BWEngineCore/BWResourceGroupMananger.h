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
enum ScriptLoadOrder // �ű���������ȼ�
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
	typedef std::map<std::string, BWResourcePtr> ResourceMap; // ʹ�����ַ�ʽ�洢��Դ�᲻����ɳ�ͻ��
	typedef std::multimap<int, BWScriptLoader*> ScriptLoaderMap;  // ��int����������ȼ� intֵԽ�� ���ȼ�Խ�� ��ͬ���ȼ�֮�䲻��ȷ��˳��  ����program��ԴҪ����material��Դ���� ������material�в�������program�еı���

	ScriptLoaderMap mScriptLoadMap;
	ResourceGroupMap resourceGroupMap;
	ResourceManagerMap resourceManagerMap;
	ResourceMap resourceMap;

	loadCollision collision; // ��ͻ����

};

#endif