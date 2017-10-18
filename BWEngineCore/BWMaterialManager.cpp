#include "BWMaterialManager.h"
#include "BWResourceGroupMananger.h"
#include "BWMaterial.h"
#include "BWParser.h"
BWMaterialManager* BWMaterialManager::GetInstance()
{
	assert(instance);
	return instance;
}
BWMaterialManager::BWMaterialManager() :
 mDefultMinFiltering(FO_LINEAR)
, mDefultMagFiltering(FO_LINEAR)
, mDefultMipfiltering(FO_LINEAR)
, mDefulutAnisotropy(1)
{
	pattern = ".material";
	order = 30;
	BWResourceGroupManager::GetInstance()->RegisterScriptLoader(this , order);
	BWResourceGroupManager::GetInstance()->RegisterResourceManager(pattern, this);
}
BWMaterialManager::~BWMaterialManager()
{

}
BWResourcePtr BWMaterialManager::CreateImp(const std::string &name, const std::string &groupName)
{
	return new BWMaterial(name, groupName);
}

void BWMaterialManager::ParseScript( BWDataStream& dataStream , const std::string &groupName)
{
	Parser::ParseMaterial(dataStream, groupName);
}
unsigned int BWMaterialManager::GetDefaultAnisotropy()
{
	return mDefulutAnisotropy;
}
void BWMaterialManager::setActiveScheme(const std::string &name)
{
	mActiveSchemeName = name;
}
FilterOptions BWMaterialManager::GetDefaultTextureFiltering(FilterType type) const
{
	switch (type)
	{
	case FT_MIN:
		return mDefultMinFiltering;
	case FT_MAG:
		return mDefultMagFiltering;
	case FT_MIP:
		return mDefultMipfiltering;
	default:
		break;
	}
	return mDefultMinFiltering;
}