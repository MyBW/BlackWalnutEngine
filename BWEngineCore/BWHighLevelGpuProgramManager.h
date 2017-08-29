#ifndef HIGHLEVELGPUPROGRAMMANAGER_H_
#define HIGHLEVELGPUPROGRAMMANAGER_H_

#include "BWSingleton.h"
#include "BWGpuProgramManager.h"
#include "BWHighLevelGpuProgram.h"
#include "BWHighLevelGpuProgramFactory.h"
const std::string NULLGpuProgramFactory = "NULL";
class BWHighLevelGpuProgramManager :  public BWResourceManager, public BWSingleton<BWHighLevelGpuProgramManager>
{
public:
	static BWHighLevelGpuProgramManager* GetInstance();
	BWHighLevelGpuProgramManager();
	bool IsSyntaxSupported(const std::string &syntax);
	virtual BWHighLevelGpuProgramPtr CreateProgram(const std::string &name, const std::string &goupName, const std::string &language, GpuProgramType type);
	bool InsertFactory( BWHighLevelGpuProgramFactory *factory);
	void RemoveFactory(const BWHighLevelGpuProgramFactory *factory);
    BWHighLevelGpuProgramFactory* GetFactory(const std::string &language);
	virtual void ParseScript(BWDataStream& dataStream , const std::string &groupName);
protected:
	virtual BWResourcePtr CreateImp(const std::string &name, const std::string &groupName);
private:
	typedef std::map< std::string,  BWHighLevelGpuProgramFactory *> GpuProgramFactoryMap;

	GpuProgramFactoryMap factoryMap;
};
#endif