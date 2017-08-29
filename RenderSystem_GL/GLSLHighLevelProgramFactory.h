#ifndef GLSLHIGHLEVELPROGRAMFACTORY_H_
#define GLSLHIGHLEVELPROGRAMFACTORY_H_
#include "../BWEngineCore/BWHighLevelGpuProgramFactory.h"
#include "../BWEngineCore/BWHighLevelGpuProgram.h"
#include "GLSLGpuProgram.h"
class GLSLHighLevelProgramFactory : public BWHighLevelGpuProgramFactory
{
public:
	const std::string  GetLanguage()
	{
		std::string type = "GLSL";
		return type;
	}
	BWHighLevelGpuProgram* CreateGpuProgram(const std::string &name, const std::string &groupName, BWResourceManager *creator)
	{
		return new GLSLGpuProgram(name, groupName, creator);
	}
protected:
private:
};

#endif