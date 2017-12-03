#include"BWGpuProgramManager.h"
#include "BWGpuProgramParams.h"


BWGpuProgramPtr BWGpuProgramManager::Create(const std::string& name, const std::string &groupName, GpuProgramType type, const std::string& syntax)
{
	BWResourcePtr resource = CreateImp(name , groupName , type , syntax);
	AddImp(resource);
	BWResourceGroupManager::GetInstance()->NotifyResourceCreated(resource);
	return dynamic_cast<BWGpuProgram*>(resource.Get());
}
BWGpuProgramPtr BWGpuProgramManager::CreateProgram(const std::string &name, const std::string &groupName, const std::string &fileName, GpuProgramType type, const std::string & syntax)
{
	BWGpuProgramPtr program = Create(name, groupName, type, syntax);
	program->SetSourceFile(fileName);
	return program;
}
BWGpuProgramParameters* BWGpuProgramManager::CreateParameters()
{
	return new BWGpuProgramParameters();
}
BWGpuSharedParametersPtr BWGpuProgramManager::CreateSharedParameters(const std::string& name)
{
	SharedParameterMap::iterator result = sharedParameterMap.find(name);
	if (result != sharedParameterMap.end())
	{
		//Òì³£
		return NULL;
	}
	BWGpuSharedParametersPtr sharedParameter = new BWGpuSharedParameters();
	sharedParameter->SetName(name);
	sharedParameterMap.insert(SharedParameterMap::value_type(name, sharedParameter));
	return sharedParameter;
}
BWGpuSharedParametersPtr BWGpuProgramManager::GetSharedParameters(const std::string &name)
{
	SharedParameterMap::iterator result = sharedParameterMap.find(name);
	if (result != sharedParameterMap.end())
	{
		return result->second;
	}
	return NULL;
}
 BWResource* BWGpuProgramManager::CreateImp(const std::string name, const std::string &groupName, GpuProgramType type, const std::string &syntax)
{
	 return NULL;
}