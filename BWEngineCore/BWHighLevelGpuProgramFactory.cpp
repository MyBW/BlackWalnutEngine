#include "BWHighLevelGpuProgramFactory.h"
#include "BWHighLevelGpuProgramManager.h"
const std::string NullhighLevelGpuProgramFactory::language = NULLGpuProgramFactory;

const std::string NullhighLevelGpuProgramFactory::GetLanguage()
{
	return language;
}

BWHighLevelGpuProgram* NullhighLevelGpuProgramFactory::CreateGpuProgram(const std::string &name, const std::string &groupName, BWResourceManager *creator)
{
	return new BWHighLevelGpuProgram(name, groupName, creator);
}

