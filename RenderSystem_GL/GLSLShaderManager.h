#ifndef GLSLSHADERMANAGER_H_
#define GLSLSHADERMANAGER_H_
#include "../BWEngineCore/BWShaderManager.h"

class GLSLShaderManager : public BWShaderManager
{
public:
	GLSLShaderManager();
	~GLSLShaderManager();
protected:
	virtual BWResourcePtr CreateImp(const std::string &name, const std::string &groupName);
private:
};



#endif




