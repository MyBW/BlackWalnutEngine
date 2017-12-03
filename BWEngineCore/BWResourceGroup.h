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
		std::string resourceName; //��Դ����
		std::string resourceType;//��Դ���� ����ȷ��resourceMananger
		//���ȷ����Դ·��
	};
	typedef std::list<Location*> LoactioinList;
	typedef std::map<std::string, BWArchive*> ResourceLoactionIndex; //���������ͻ����Ϊ�ļ��������˺�׺ ���������һ�����ļ��� ˵����Դ�غ� ��Ҫ������Դ���÷�ʽ
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