#ifndef GPUPROGRAMMANAGER_H_
#define GPUPROGRAMMANAGER_H_
#include "BWSingleton.h"
#include "BWResourceManager.h"
#include "BWGpuProgram.h"
#include "BWGpuSharedParameters.h"
class BWGpuProgramManager : public BWResourceManager , public BWSingleton<BWGpuProgramManager>
{
public:
	BWGpuProgramPtr GetByName(const std::string& name);
	bool IsSyntaxSupported(const std::string &);

	virtual BWGpuProgramPtr Create(const std::string& name, const std::string &groupName, GpuProgramType type, const std::string& syntax);
	virtual BWGpuProgramPtr CreateProgram(const std::string &name, const std::string &groupName, const std::string &fileName, GpuProgramType type , const std::string & syntax);
	BWGpuSharedParametersPtr CreateSharedParameters(const std::string& name);
	BWGpuProgramParameters* CreateParameters();
	BWGpuSharedParametersPtr GetSharedParameters(const std::string &name);
protected:
	virtual BWResource* CreateImp(const std::string name, const std::string &groupName, GpuProgramType type, const std::string &syntax) ;
private:
	typedef std::map<std::string, BWGpuSharedParametersPtr> SharedParameterMap;
	SharedParameterMap sharedParameterMap;
};
#endif