#include "BWGpuProgramUsage.h"
#include "BWHighLevelGpuProgramManager.h"
const std::string & BWGpuProgramUsage::GetProgramName()
{
	return gpuProgram->GetResourceName();
}
void BWGpuProgramUsage::SetGpuProgram(const std::string &name, bool isHighLevelGpuProgram)
{
	if (!gpuProgram.IsNull())
	{
		if (gpuProgram->GetResourceName() == name)
		{
			return;
		}
	}
	if (isHighLevelGpuProgram == true)
	{
		BWGpuProgramPtr tmpProgram = BWHighLevelGpuProgramManager::GetInstance()->GetResource(name , mParent->GetGroupName());
		if (tmpProgram.IsNull())
		{
			assert(0);
		}
		gpuProgram = tmpProgram;
	}
	else
		{
		assert(0);
	}
	gpuProgramType = gpuProgram->GetType();
	BWGpuProgramParametersPtr oldGpuProgramParameter = gpuProgramParameter;
	gpuProgramParameter = gpuProgram->CreateParameter();
	if (!oldGpuProgramParameter.IsNull())
	{
		gpuProgramParameter->CopyMatchingNamedConstantForm((*oldGpuProgramParameter.Get()));
	}
}
void BWGpuProgramUsage::SetGpuProgram(BWGpuProgramPtr gpuProgram)
{
	this->gpuProgram = gpuProgram;
	if (!gpuProgram.IsNull())
	{
		gpuProgramType = gpuProgram->GetType();
		BWGpuProgramParametersPtr oldGpuProgramParameter = gpuProgramParameter;
		gpuProgramParameter = gpuProgram->CreateParameter();
		if (!oldGpuProgramParameter.IsNull())
		{
			gpuProgramParameter->CopyMatchingNamedConstantForm((*oldGpuProgramParameter.Get()));
		}
		return;
	}
	gpuProgramParameter = NULL;

}
void BWGpuProgramUsage::SetGpuProgramParameter(BWGpuProgramParametersPtr gpuProgramParameter)
{
	this->gpuProgramParameter = gpuProgramParameter;
}
BWGpuProgramParametersPtr BWGpuProgramUsage::GetGpuProgramParameter()
{
	return gpuProgramParameter;
}
BWGpuProgramPtr BWGpuProgramUsage::GetGpuProgram()
{
	return gpuProgram;
}
void BWGpuProgramUsage::setGPUProgram(const std::string &name , const std::string &language) // 这个GLSL已经失去意义
{
	if (!gpuProgram.IsNull())
	{
		if (gpuProgram->GetResourceName() == name)
		{
			return;
		}
	}
	BWGpuProgramPtr tmpProgram = BWHighLevelGpuProgramManager::GetInstance()->GetResource(name, mParent->GetGroupName());
	if (tmpProgram.IsNull())
	{
		assert(0);
	}
	gpuProgram = tmpProgram;
	gpuProgramType = gpuProgram->GetType();
	BWGpuProgramParametersPtr oldGpuProgramParameter = gpuProgramParameter;
	gpuProgramParameter = gpuProgram->CreateParameter();
	if (!oldGpuProgramParameter.IsNull())
	{
		gpuProgramParameter->CopyMatchingNamedConstantForm((*oldGpuProgramParameter.Get()));
	}
}