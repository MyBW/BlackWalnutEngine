#ifndef GPUPROGRAMUSAGE_H_
#define GPUPROGRAMUSAGE_H_
#include "BWGpuProgramParams.h"
#include "BWGpuProgram.h"
#include "BWHighLevelGpuProgram.h"
class BWPass;
class BWGpuProgramUsage 
{
public:
	BWGpuProgramUsage() :mParent(NULL), gpuProgramParameter(NULL), gpuProgram(NULL)
	{
	}
	BWGpuProgramUsage(BWPass* parent) :mParent(parent), gpuProgramParameter(NULL), gpuProgram(NULL)
	{
			
	}
	~BWGpuProgramUsage()
	{
		mParent = NULL;
	}
	BWGpuProgramParametersPtr GetGpuProgramParameter();
	BWGpuProgramPtr GetGpuProgram();
	BWHighLevelGpuProgramPtr GetHighLevelGpuProgram(); // 其实程序内部都是使用的HighLevel
	const std::string& GetProgramName();
	void SetGpuProgram(BWGpuProgramPtr gpuProgram);
	void SetGpuProgram(const std::string &name, bool isHighLevelGpuProgram = true);
	void SetGpuProgramParameter(BWGpuProgramParametersPtr gpuProgramParameter);
	void SetParent(BWPass * parent){ mParent = parent; }

	void setGPUProgram(const std::string &name , const std::string &language);
private:
	BWPass* mParent;
	BWGpuProgramParametersPtr gpuProgramParameter;
	BWGpuProgramPtr gpuProgram;
	GpuProgramType gpuProgramType;
};
//class BWGpuProgramUsagePtr : public SmartPointer<BWGpuProgramUsage>
//{
//public:
//	BWGpuProgramUsagePtr() :SmartPointer<BWGpuProgramUsage>() {}
//	BWGpuProgramUsagePtr(BWGpuProgramUsage* ptr) :SmartPointer<BWGpuProgramUsage>(ptr) {}
//	BWGpuProgramUsagePtr(const BWResourcePtr& resource)
//	{
//		mPointer = dynamic_cast<BWGpuProgramUsage*>(resource.Get());
//		counter = resource.GetCounterPointer();
//		(*counter)++;
//	}
//	const BWGpuProgramUsagePtr& operator=(BWResourcePtr resource)
//	{
//		if (mPointer == dynamic_cast<BWGpuProgramUsage*> (resource.Get()))
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
//		mPointer = dynamic_cast<BWGpuProgramUsage*>(resource.Get());
//		counter = resource.GetCounterPointer();
//	}
//};

#endif