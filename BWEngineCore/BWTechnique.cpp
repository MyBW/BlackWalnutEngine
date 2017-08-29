#include "BWTechnique.h"
#include "BWMaterialManager.h"
#include "BWPass.h"
BWTechnique::BWTechnique() :parent(NULL), LODIndex(-1)
{

}
BWTechnique::~BWTechnique()
{
	PassList::iterator itor = passList.begin();
	while (itor != passList.end())
	{
		delete (*itor);
		itor++;
	}
	passList.clear();
}
void BWTechnique::SetName(const std::string &name)
{
	this->name = name;
}
void BWTechnique::SetSchemeName(const std::string& name)
{
	schemeName = name;
}

void BWTechnique::SetParent(BWMaterial *Parent)
{
	this->parent = Parent;
}
void BWTechnique::SetLodIndex(unsigned short index)
{
	LODIndex = index;
}

void BWTechnique::SetShadowCasterMaterial(const std::string &material)
{
	shadowCasterMaterial =dynamic_cast<BWMaterial*>(BWMaterialManager::GetInstance()->GetResource(material).Get());
}
void BWTechnique::SetShadowReceiverMaterial(const std::string &material)
{
	shadowReceiverMaterial = dynamic_cast<BWMaterial*>(BWMaterialManager::GetInstance()->GetResource(material).Get());
}
void BWTechnique::AddGPUDeviceNameRule(const GPUDeviceNameRule& deviceName)
{
	gpuDeviceNameRule = deviceName;
}
void BWTechnique::AddGPUVendorRule(const GPUVendorRule& vendorRule)
{
	gpuVendorRule = vendorRule;
}
BWPass* BWTechnique::CreatePass()
{
	BWPass* pass = new BWPass();
	pass->SetParent(this);
	pass->setIndex(passList.size());
	passList.push_back(pass);
	return pass;
}
bool BWTechnique::isLoaded()
{
	return parent->isLoad();
}
bool BWTechnique::isTransparent()
{
	return false;
}
 const BWTechnique::PassList&  BWTechnique::getPassList() const
{
	return passList;
}

 void BWTechnique::load()
 {
	 PassList::iterator itor = passList.begin();
	 PassList::iterator end = passList.end();
	 while (itor != end)
	 {
		 (*itor)->load();
		 itor++;
	 }
 }

 BWPass*  BWTechnique::GetPass(int i)
 {
	  return passList[i];
 }