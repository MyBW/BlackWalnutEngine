#include "GLVAOManager.h"

GLVAOManager* GLVAOManager::Getinstance()
{
	assert(instance);
	return instance;
}
GLVAOManager::GLVAOManager()
{

}
GLVAOManager::~GLVAOManager()
{

}
GLVAO* GLVAOManager::findVAO(GLSLGpuProgram *program, IndexDataPrt indexData, VertexDataPrt vertexData)
{
	VAOMap::iterator itor = mVAOMap.begin();
	VAOInfor *infor = NULL;
	while (itor != mVAOMap.end())
	{
		 infor = itor->first;
		if (infor->mProgram == program && infor->mIndexData.Get() == indexData.Get() && infor->mVertexData.Get() == vertexData.Get())
		{
			return itor->second;
		}
		itor++;
	}
	return NULL;
}
GLVAO* GLVAOManager::createVAO(GLSLGpuProgram *program, IndexDataPrt indexdata, VertexDataPrt vertexData)
{
	GLVAO * vao = findVAO(program, indexdata, vertexData);
	if (!vao)
	{
		VAOInfor *infor = new VAOInfor;
		infor->mProgram = program;
		infor->mIndexData = indexdata;
		infor->mVertexData = vertexData;
		vao = new GLVAO();
		vao->init(program, indexdata , vertexData);
		mVAOMap.insert(VAOMap::value_type(infor, vao));
		return vao;
	}
	return vao;
}
bool GLVAOManager::releaseVAO(GLSLGpuProgram *program)
{
	VAOMap::iterator itor = mVAOMap.begin();
	while (itor != mVAOMap.end())
	{
		if (itor->first->mProgram == program)
		{
			itor->first->mProgram = NULL;
			delete itor->second;
			delete itor->first;
			itor = mVAOMap.erase(itor);
			continue;
		}
		itor++;
	}
	return true;
}