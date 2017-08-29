#ifndef MATERIALMANAGER_H_
#define MATERIALMANAGER_H_

#include "BWResourceManager.h"
#include "BWMaterial.h"
class BWMaterialManager : public BWResourceManager , public BWSingleton<BWMaterialManager>
{
public:
	static BWMaterialManager* GetInstance();
	BWMaterialManager();
	~BWMaterialManager();
	virtual void ParseScript( BWDataStream& dataStream , const std::string &groupName);
	unsigned int  GetDefaultAnisotropy();
	virtual FilterOptions GetDefaultTextureFiltering(FilterType type) const;
	void setActiveScheme(const std::string &name);
protected:
	virtual BWResourcePtr CreateImp(const std::string &name, const std::string &groupName);
	unsigned int mDefulutAnisotropy;
	std::string mActiveSchemeName;
	FilterOptions mDefultMinFiltering;
	FilterOptions mDefultMagFiltering;
	FilterOptions mDefultMipfiltering;
};

#endif