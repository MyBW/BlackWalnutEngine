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
	//ģ�巽��
	virtual BWResourcePtr CreateImp(const std::string &name , const std::string &groupName);
	virtual void AddImp(BWResourcePtr resource);
protected:
	typedef std::map<std::string, BWResourcePtr> ResourceMap;
	typedef std::map<std::string, ResourceMap> ResourceWithGroup;
	typedef std::map<int, BWResourcePtr> ResouerceHandleMap;
	
	//����������ִ�����Ը�ķ�ʽ һ���ǽ���Դ���� resourcegroupmanager����Դ���� ����ͨ��resourcegroupmanager��������Դ
	//��һ����ͨ��resourcemanager ��������Դ  ������Ӧ������ʱ  ������Դ  ��ʱ��Դ�鲢��һ����resourcegroupmanager��
	//��ʱ ����Ҫ����Դ���� resourcewithgroup��  
	ResourceMap resourceMap; //���map��resourcegroupmanager����Դ���е����ݱ���һ��
	ResourceWithGroup  resourceWithGroup;//�Լ�����Դ��
	ResouerceHandleMap resourceHnadleMap;// ͨ��������������Դ

	std::string resourceType;
};

#endif