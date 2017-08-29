#ifndef GPUPROGRAMUSAGE_H_
#define GPUPROGRAMUSAGE_H_
#include "BWGpuProgramParams.h"
#include "BWGpuProgram.h"
#include "BWPass.h"
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


#endif