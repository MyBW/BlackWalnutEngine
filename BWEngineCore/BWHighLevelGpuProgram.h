#ifndef HIGHLEVELGPUPROGRAM_H_
#define HIGHLEVELGPUPROGRAM_H_
#include "BWResourceManager.h"
#include "AllSmartPointRef.h"
#include "BWGpuProgram.h"
#include <string>
#include "BWShader.h"
#include "BWVertexIndexData.h"
enum GpuConstantType;
class BWHighLevelGpuProgram  : public BWGpuProgram
{
public:
	BWHighLevelGpuProgram(const std::string &name, const std::string &goupName, BWResourceManager *creator) :BWGpuProgram(name, goupName, creator),mLinkeFinish(false),highLevelLoaded(false)
	{

	}
	virtual ~BWHighLevelGpuProgram() { }
	virtual void LoadHighLevel();
	virtual void UnLoadHighLevel();
	virtual void LoadFromSource();
	virtual BWGpuProgramParametersPtr CreateParameter();
	GpuNamedConstantsPtr GetNamedConstantsDefinition();
	virtual void PopulateParameter(BWGpuProgramParametersPtr programParameter);
	virtual void SetGPUProgramParameters(BWGpuProgramParametersPtr ProgramParameters);
	virtual void SetGlobalUniformBufferObject(BWGpuProgramParametersPtr ProgramParameters);
	virtual void InitNamedConstantes();
	size_t calculateSize(){ return 0; }

	virtual bool  addShader(BWShaderPtr shader) ;
	virtual bool  SetParameter(const std::string &name, void *value , GpuConstantType ConstantType) { return false; }
	void SetCoputerShaderDimentsion(int InDimentsionX, int InDimentsionY, int InDimentsionZ);
protected:
	void prepareImpl(){ }
	void preLoadImpl(){ }
	void loadImpl(){ }
	void postLoadImpl() { }
	virtual void LoadHighLevelImp();
	/* Ϊ�˲���  ��ʱ��Э
	virtual void UnLoadHighLevelImp() = 0;*/
	

	virtual void UnLoadHighLevelImp();
	bool highLevelLoaded;
	typedef std::vector<BWShaderPtr> ShaderList;
	ShaderList mShaderList;
	bool mLinkeFinish;
	int DimentsionX{ 1 };
	int DimentsionY{ 1 };
	int DimentsionZ{ 1 };
};


//class BWHighLevelGpuProgramPtr : public SmartPointer<BWHighLevelGpuProgram>
//{
//public:
//	BWHighLevelGpuProgramPtr() :SmartPointer<BWHighLevelGpuProgram>(){}
//	BWHighLevelGpuProgramPtr(BWHighLevelGpuProgram* ptr) :SmartPointer<BWHighLevelGpuProgram>(ptr){}
//	BWHighLevelGpuProgramPtr(const BWResourcePtr& resource)
//	{
//		mPointer = dynamic_cast<BWHighLevelGpuProgram*>(resource.Get());
//		counter = resource.GetCounterPointer();
//		(*counter)++;
//
//	}
//	const BWHighLevelGpuProgramPtr& operator=(BWResourcePtr resource)
//	{
//		if (mPointer == dynamic_cast<BWHighLevelGpuProgram*> (resource.Get()))
//		{
//			return *this;
//		}
//		if (mPointer)
//		{
//			(*counter)--;
//			if ((*counter) == 0)
//			{
//				delete mPointer;
//			}
//		}
//		mPointer = dynamic_cast<BWHighLevelGpuProgram*>(resource.Get());
//		counter = resource.GetCounterPointer();
//		(*counter)++;
//	}
//	const BWHighLevelGpuProgramPtr& operator=(BWGpuProgramPtr resource)
//	{
//		if (mPointer == dynamic_cast<BWHighLevelGpuProgram*> (resource.Get()))
//		{
//			return *this;
//		}
//		if (mPointer)
//		{
//			(*counter)--;
//			if ((*counter) == 0)
//			{
//				delete mPointer;
//			}
//		}
//		mPointer = dynamic_cast<BWHighLevelGpuProgram*>(resource.Get());
//		counter = resource.GetCounterPointer();
//		(*counter)++;
//	}
//
//
//};

#endif