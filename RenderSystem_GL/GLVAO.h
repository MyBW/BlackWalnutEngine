#ifndef GLVAO_H_
#define GLVAO_H_
#include <map>
#include "GL/include/glew.h"
#include "../BWEngineCore/BWVertexIndexData.h"
typedef std::map<std::string, GLint> NameLocation;
class GLSLGpuProgram;
class GLVAO
{
public:
	GLVAO();
	~GLVAO();
	void           init(GLSLGpuProgram *program , IndexDataPrt indexData , VertexDataPrt vertexData);
	GLint          getGLID();
	void           bind();
protected:
	void           relaseGLBuffer();
private:
	GLuint  mID;
	GLSLGpuProgram *mProgram;
};


#endif