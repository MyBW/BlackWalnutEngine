#include "BWHighLevelGpuProgramManager.h"
#include "BWResourceGroupMananger.h"
#include "BWStringOperation.h"
#include "BWParser.h"
BWHighLevelGpuProgramManager* BWHighLevelGpuProgramManager::GetInstance()
{
	assert(instance);
	return instance;
}

BWHighLevelGpuProgramManager::BWHighLevelGpuProgramManager()
{
	InsertFactory(new NullhighLevelGpuProgramFactory());
	pattern = ".program";
	order = SLO_2;
	BWResourceGroupManager::GetInstance()->RegisterScriptLoader(this, order);
	BWResourceGroupManager::GetInstance()->RegisterResourceManager(pattern, this);

}

BWHighLevelGpuProgramPtr BWHighLevelGpuProgramManager::CreateProgram(const std::string &name, const std::string &goupName, const std::string &language, GpuProgramType type)
{
	BWHighLevelGpuProgramFactory* factory = GetFactory(language);
	BWResourcePtr gpuProgram = factory->CreateGpuProgram(name, goupName, this);
	AddImp(gpuProgram);
	BWResourceGroupManager::GetInstance()->NotifyResourceCreated(gpuProgram);
	BWHighLevelGpuProgramPtr highProgram = gpuProgram;
	highProgram->SetType(type);
	highProgram->SetSyntaxCode(language);
	return highProgram;
}
BWResourcePtr BWHighLevelGpuProgramManager::CreateImp(const std::string &name, const std::string &groupName)
{
	return NULL;
}
  BWHighLevelGpuProgramFactory* BWHighLevelGpuProgramManager::GetFactory(const std::string &language)
{
	GpuProgramFactoryMap::iterator  factory = factoryMap.find(language);
	if (factory == factoryMap.end())
	{
		//如果没有支持的语言 则使用默认的语言
		return factoryMap.find(NULLGpuProgramFactory)->second;
	}
	return factory->second;
}
bool BWHighLevelGpuProgramManager::InsertFactory( BWHighLevelGpuProgramFactory *factory)
{
   if (factory == NULL)
   {
	   return false;
   }
   std::pair<GpuProgramFactoryMap::iterator, bool> result = factoryMap.insert(GpuProgramFactoryMap::value_type(factory->GetLanguage(), factory));
   return result.second;
}
void BWHighLevelGpuProgramManager::ParseScript(BWDataStream& dataStream , const std::string &groupName)
{
	if (!Parser::parseGPUProgram(dataStream, groupName))
	{
		assert(0);
	}
}