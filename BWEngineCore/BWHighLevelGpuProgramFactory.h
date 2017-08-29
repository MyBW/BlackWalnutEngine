#ifndef HIGHLEVLGPUPROGRAMFACTORY_H_
#define HIGHLEVLGPUPROGRAMFACTORY_H_
#include <string>
#include "BWGpuProgram.h"
#include "BWHighLevelGpuProgram.h"
#include "BWResourceManager.h"

class BWHighLevelGpuProgramFactory 
{
public:
	virtual const std::string  GetLanguage() = 0;
	virtual BWHighLevelGpuProgram* CreateGpuProgram(const std::string &name, const std::string &groupName, BWResourceManager *creator) = 0 ;
protected:
private:
};

class  NullhighLevelGpuProgramFactory : public BWHighLevelGpuProgramFactory
{
public: 
	virtual const std::string GetLanguage();
	virtual BWHighLevelGpuProgram* CreateGpuProgram(const std::string &name, const std::string &groupName, BWResourceManager *creator);
private:
	static const std::string language;
};
#endif