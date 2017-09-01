#ifndef RESOURCE_H_
#define RESOURCE_H_
#include "BWSmartPointer.h"
#include "BWStringInterface.h"
#include <string>
class BWResourceManager;

enum LoadingState
{
	LOADSTATE_UNLOADED,
	LOADSTATE_LOADING,
	LOADSTATE_LOADED,
	LOADSTATE_UNLOADING,
	LOADSTATE_PREPARED,
	LOADSTATE_PREPARING
};
class BWResource : public BWStringInterface
{
public:
	BWResource();
	BWResource(BWResourceManager *mamager, const std::string &resoucename, const std::string &groupname) :
		creator(mamager)
		,name(resoucename)
		, groupName(groupname)
		, mStateCount(0)
		,mLoadState(LOADSTATE_UNLOADED)
	{
		id = ID;
		ID++;
	}
	BWResource(const std::string &resoucename, const std::string &groupname) :name(resoucename), groupName(groupname)
	{
		id = ID;
		ID++;
	}
	virtual ~BWResource();
	void   NotifyOrigin(const std::string& origin) { this->origin = origin; }
	void   SetResourceName(const std::string &resourceName){ name = resourceName; }
	void   SetGroupName(const std::string &groupName) { this->groupName = groupName; }
	void   SetCreator(BWResourceManager* resourceMananger){ creator = resourceMananger; }
	const std::string& GetResourceName();
	const std::string& GetGroupName();
	size_t getStateCount(){ return mStateCount; }
	LoadingState getLoadingState() const{	return mLoadState; }
	const BWResourceManager* GetCreator(){ return creator; }
	virtual void Load(); //模板方法
	virtual void unLoad();
	bool isLoad();
	int  GetHIID();
	void Prepare(bool backthread = false);
	virtual void touch();
	virtual size_t calculateSize() = 0;
	const std::string& getName() const { return name; }
protected:
	void _dirtyState();
	virtual void prepareImpl() = 0;
 	virtual void preLoadImpl() = 0;
	virtual void loadImpl() = 0 ;
	virtual void postLoadImpl() = 0;
	virtual void unloadImp();
	static int ID;
	int id;
	std::string name;
	std::string groupName;
	std::string origin;
	LoadingState mLoadState;
	BWResourceManager* creator;
	size_t mSize;  // 如果该资源是texture的话 并没有包含mipmap占用的空间
	unsigned short mStateCount; // 资源被load的次数

};
typedef SmartPointer<BWResource> BWResourcePtr;


#endif