#ifndef GLSLSHADER_H_
#define GLSLSHADER_H_

#include "../BWEngineCore/BWShader.h"
#include "../BWEngineCore/BWGpuProgramParams.h"
#include "GLSLShaderManager.h"
#include "GLUBO.h"
#include "GLVAO.h"

class GLSLShader : public BWShader
{
public:
	GLSLShader(GLSLShaderManager *manager, const std::string &name,const std::string &groupName);
	~GLSLShader();
	virtual size_t calculateSize(){ return 0; }
    
	GLenum  shaderGLType() const;
	void    createNamedConstant();
	GpuNamedConstantsPtr  getNamedConstant() const;
	UBOInforMap  getUBOInfor()  ;
    NameLocation getInConstant() ;
	NameLocation GetFragmentOutConstant();
	GLuint GetHIID() { return mID; }
protected:
	virtual void prepareImpl(){ }
	virtual void preLoadImpl();
	virtual void loadImpl();
	virtual void postLoadImpl(){ }
	bool  compileShader();   
	void  createUniformConstant(std::string &uniformSentence);
	void  createUniformBuffer(std::string &uniformBuffer);
	void  createInConstant(std::string &inConstant);
	void  createOutConstant(std::string& outCosntant);
private:
	GLuint mID;
	GpuNamedConstantsPtr namedConstantes;
	UBOInforMap mUBOInfor;
	NameLocation mNameLoaction;
	NameLocation mFragOutNameLocation;
	bool mIsCreateNamedConstant;
};

typedef SmartPointer<GLSLShader> GLSLShaderPrt;

#endif


