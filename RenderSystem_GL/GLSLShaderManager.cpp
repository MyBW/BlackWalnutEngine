#include "GLSLShaderManager.h"
#include "GLSLShader.h"
GLSLShaderManager::GLSLShaderManager()
{

}
GLSLShaderManager::~GLSLShaderManager()
{

}

BWResourcePtr GLSLShaderManager::CreateImp(const std::string &name, const std::string &groupName)
{
	GLSLShaderPrt shader = new GLSLShader(this, name, groupName);
	return shader;
}