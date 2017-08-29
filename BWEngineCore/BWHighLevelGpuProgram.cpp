#include "BWHighLevelGpuProgram.h"
#include "BWGpuProgramManager.h"
#include "BWLog.h"
BWGpuProgramParametersPtr BWHighLevelGpuProgram::CreateParameter()
{
	BWGpuProgramParametersPtr programParam = BWGpuProgramManager::GetInstance()->CreateParameters();
	if (this->IsSupported())
	{
		LoadHighLevel();
		PopulateParameter(programParam);
	}
	if (!defaultPararmeter.IsNull())
	{
		programParam->CopyConstantsFrom((*defaultPararmeter.Get()));
	}
	return programParam;
}
GpuNamedConstantsPtr BWHighLevelGpuProgram::GetNamedConstantsDefinition()
{
	if (!isInitNamedConstantes)
	{
		InitNamedConstantes();
		isInitNamedConstantes = true;
	}
	return namedConstantes;
}
void BWHighLevelGpuProgram::PopulateParameter(BWGpuProgramParametersPtr programParameter)
{
	GetNamedConstantsDefinition();
	programParameter->SetNamedConstants(namedConstantes);
	programParameter->SetLogicalIndexes(intLogicalToPhysical, floatLogicalToPhysical);
}
void BWHighLevelGpuProgram::InitNamedConstantes()
{

}

void BWHighLevelGpuProgram::LoadHighLevel()
{
	if (!highLevelLoaded)
	{
		LoadHighLevelImp();
		highLevelLoaded = true;
		if (!defaultPararmeter.IsNull())
		{
			BWGpuProgramParametersPtr save = defaultPararmeter;
			defaultPararmeter.SetNull();
			defaultPararmeter = CreateParameter();
			defaultPararmeter->CopyMatchingNamedConstantForm(*(save.Get()));
		}
	}
}
void BWHighLevelGpuProgram::UnLoadHighLevel()
{
	if (highLevelLoaded)
	{
		UnLoadHighLevelImp();
		isInitNamedConstantes = false;
		CreateParameterMappingStructure(true);
		highLevelLoaded = false;
	}
}
void BWHighLevelGpuProgram::LoadHighLevelImp()
{
	if (isLoadFileName)
	{
		BWDataStreamPrt data = BWResourceGroupManager::GetInstance()->OpenResource(sourcefile, this->GetGroupName());
		source = data->GetAsString();
	}
	LoadFromSource();
}
void BWHighLevelGpuProgram::UnLoadHighLevelImp()
{
	
}
void BWHighLevelGpuProgram::LoadFromSource()
{

}

bool BWHighLevelGpuProgram::addShader(BWShaderPtr shader)
{
	ShaderList::iterator itor = mShaderList.begin();
	while (itor != mShaderList.end())
	{
		if ((*itor)->getShaderType() == shader->getShaderType())
		{
			Log::GetInstance()->logMessage("GLSLGPUProgram::addShader  : this type is already included. \n");
			assert(0);
			return false;
		}
		itor++;
	}
	mShaderList.push_back(shader);
}