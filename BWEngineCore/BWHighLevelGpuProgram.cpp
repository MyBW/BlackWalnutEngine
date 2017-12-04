#include "BWHighLevelGpuProgram.h"
#include "BWGpuProgramManager.h"
#include "BWLog.h"
#include "AllSmartPointRef.h"
#include "BWGpuProgramUsage.h"
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

void BWHighLevelGpuProgram::SetGPUProgramParameters(BWGpuProgramParametersPtr ProgramParameters)
{
	GpuNamedConstantsPtr namedConstant = ProgramParameters->getNamedConstants();
	if (namedConstant.IsNull())
	{
		namedConstant = GetDefaultParameters()->getNamedConstants();
	}
	SetGlobalUniformBufferObject(ProgramParameters);
	//设置各种unifomr参数
	std::for_each(namedConstant->map.begin(), namedConstant->map.end(),[this ,ProgramParameters](GpuConstantDefinitionMap::reference NamedConstant) {

		if (!(ProgramParameters->IsHaveGlobalUniformBufferObject()
			&& ProgramParameters->IsGlobalUniformBufferHaveTheMember(NamedConstant.first)))
		{
			GpuConstantDefinition *def = &(NamedConstant.second);
			void *data = NULL;
			if (def->isFloat())
			{
				data = ProgramParameters->GetFloatPointer(def->physicalIndex);
			}
			else
			{
				data = ProgramParameters->GetIntPointer(def->physicalIndex);
			}
			SetParameter(NamedConstant.first, data, def->constType);
		}
	});

}

void BWHighLevelGpuProgram::SetGlobalUniformBufferObject(BWGpuProgramParametersPtr ProgramParameters)
{

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

void BWHighLevelGpuProgram::SetCoputerShaderDimentsion(int InDimentsionX, int InDimentsionY, int InDimentsionZ)
{
	InDimentsionX <= 0 ? DimentsionX = 1 : DimentsionX = InDimentsionX;
	InDimentsionY <= 0 ? DimentsionY = 1 : DimentsionY = InDimentsionY;
	InDimentsionZ <= 0 ? DimentsionZ = 1 : DimentsionZ = InDimentsionZ;
}
