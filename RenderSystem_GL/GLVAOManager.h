#ifndef GLVAOMANAGER_H_
#define GLVAOMANAGER_H_
#include "GLSLGpuProgram.h"
#include "GLVAO.h"

class GLVAOManager : public BWSingleton<GLVAOManager>
{
public:
	struct VAOInfor
	{
		GLSLGpuProgram *mProgram;
		IndexDataPrt   mIndexData;
		VertexDataPrt  mVertexData;
	};
	GLVAOManager();
	~GLVAOManager();
	static GLVAOManager* Getinstance();
	GLVAO*  findVAO(GLSLGpuProgram *program, IndexDataPrt indexData, VertexDataPrt vertexData);
	GLVAO*  createVAO(GLSLGpuProgram *program, IndexDataPrt indexdata, VertexDataPrt vertexData);
	bool    releaseVAO(GLSLGpuProgram *program);
private:
	typedef std::map<VAOInfor*, GLVAO*> VAOMap;
	VAOMap mVAOMap;
};
#endif