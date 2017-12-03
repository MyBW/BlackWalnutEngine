#ifndef	MATERIAL_H_
#define MATERIAL_H_

#include "AllSmartPointRef.h"
#include <vector>
#include "BWCommon.h"
#include "BWResource.h"
class BWTechnique;
class BWMaterial : public BWResource 
{
public:
	typedef std::vector<float> LodValueList;
	BWMaterial() { }
	BWMaterial(const std::string& name, const std::string &groupanme) :BWResource(name , groupanme)
	{

	}
	~BWMaterial();
	void RemoveAllTechniques();
	void SetLodValue(LodValueList&);
	void SetReceiveShadows(bool);
	void SetTransparencyCastsShadows(bool);
	void ApplyTextureAliases(AliasTextureNamePairList&);
	int GetNumTechniques();
	BWTechnique* getTechnique(int);
	BWTechnique* CreateTechnique();
	size_t calculateSize(){ return 0; }
protected:
	void preLoadImpl(){}
	void postLoadImpl(){}
	void loadImpl();
	void prepareImpl(){ }
private:
	typedef std::vector<BWTechnique*> Techniques;
	Techniques techniques;
};
class BWMaterialPtr : public SmartPointer<BWMaterial>
{
public:
	BWMaterialPtr()
	{

	}
	BWMaterialPtr(BWMaterial *material) :SmartPointer<BWMaterial>(material)
	{

	}
	BWMaterialPtr(const BWResourcePtr &resource)
	{
	  if (resource.IsNull())
	  {
		  mPointer = NULL;
		  counter = NULL;
		  return;
	  }
	  mPointer = dynamic_cast<BWMaterial*>(resource.Get());
	  counter = resource.GetCounterPointer();
	  (*counter)++;
	}

};

#endif