#include "BWResource.h"
#include "BWCommon.h"

int BWResource::ID = 0;
BWResource::BWResource() :creator(NULL)
{
	id = ID;
	++ID;
}
BWResource::~BWResource()
{

}
void BWResource::Prepare(bool backthread)
{

}
void BWResource::touch()
{
	if (!isLoad())
	{
		Load();
	}
}
int BWResource::GetID()
{
	return id;
}
const std::string& BWResource::GetResourceName()
{
	return name;
}
const std::string& BWResource::GetGroupName()
{
	return groupName;
}
void BWResource::_dirtyState()
{
	mStateCount++;
}
void BWResource::Load()
{
	LoadingState old = mLoadState;
	if (old == LOADSTATE_UNLOADED)
	{
		prepareImpl();
	}
	preLoadImpl();
	if (mLoadState  != LOADSTATE_LOADED)
	{
		loadImpl();
	}
	_dirtyState();
	postLoadImpl();
	mSize = calculateSize();
	mLoadState = LOADSTATE_LOADED;
}
void BWResource::unLoad()
{
	unloadImp();
	mLoadState = LOADSTATE_UNLOADED;
}
bool BWResource::isLoad()
{
	return mLoadState == LOADSTATE_LOADED;
}
void BWResource::unloadImp()
{

}