#ifndef TECHNOLOGY_H_
#define TECHNOLOGY_H_

#include <vector>
#include "BWMaterial.h"
class BWPass;
class BWTechnique	
{
	friend class BWMaterial;
public:
	enum GPUVendor
	{
		GPU_UNKNOWN = 0,
		GPU_NVIDIA = 1,
		GPU_ATI = 2,
		GPU_INTEL = 3,
		GPU_S3 = 4,
		GPU_MATROX = 5,
		GPU_3DLABS = 6,
		GPU_SIS = 7,
		GPU_IMAGINATION_TECHNOLOGIES = 8,
		GPU_APPLE = 9,  // Apple Software Renderer
		GPU_NOKIA = 10,
		GPU_MS_SOFTWARE = 11, // Microsoft software device
		GPU_MS_WARP = 12, // Microsoft WARP (Windows Advanced Rasterization Platform) software device - http://msdn.microsoft.com/en-us/library/dd285359.aspx

		/// placeholder
		GPU_VENDOR_COUNT = 13
	};
	enum IncludeOrExclude
	{
		INCLUDE ,
		EXCLUDE
	};
	struct GPUVendorRule
	{
		GPUVendor vendor;
		IncludeOrExclude includeOrExclude;
		GPUVendorRule() :vendor(GPU_UNKNOWN), includeOrExclude(EXCLUDE)
		{}
		GPUVendorRule(GPUVendor ve, IncludeOrExclude in) :vendor(ve), includeOrExclude(in)
		{}
	};
	struct GPUDeviceNameRule
	{
		std::string devicePattern;
		IncludeOrExclude includeorExclude;
		bool caseSensitive;
		GPUDeviceNameRule() :includeorExclude(EXCLUDE), caseSensitive(false)
		{}
		GPUDeviceNameRule(const std::string& pattern, IncludeOrExclude in, bool caseSen) :devicePattern(pattern), includeorExclude(in), caseSensitive(caseSen)
		{}
	};
public:
	typedef std::vector<BWPass*> PassList;

	BWTechnique();
	~BWTechnique();
	bool isTransparent();
	const PassList& getPassList() const;
	BWPass* CreatePass();
	void SetName(const std::string &name);
	void SetSchemeName(const std::string&  name);
	void SetLodIndex(unsigned short);
	void SetShadowCasterMaterial(const std::string&);
	void SetShadowReceiverMaterial(const std::string&);
	void AddGPUVendorRule(const GPUVendorRule&);
	void AddGPUDeviceNameRule(const GPUDeviceNameRule&);	
	int GetPassNum();
	BWPass* GetPass(int );
	BWMaterial* GetParent() const { return parent; };
	bool isLoaded();
	void SetParent(BWMaterial *Parent);
	std::string	GetGroupName() { return parent->GetGroupName(); }
private:
	void load();
	std::string name;
	std::string schemeName;
	BWMaterial *parent;
	PassList passList;
	unsigned int LODIndex;
	GPUVendorRule gpuVendorRule;
	GPUDeviceNameRule gpuDeviceNameRule;
	BWMaterialPtr shadowCasterMaterial;
	BWMaterialPtr shadowReceiverMaterial;
};
#endif