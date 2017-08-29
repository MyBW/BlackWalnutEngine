#ifndef GLSLGPUPROGRAM_H_
#define GLSLGPUPROGRAM_H_
#include "../BWEngineCore/BWHighLevelGpuProgram.h"
#include "GL/include/glew.h"
#include "GL/include/glut.h"
#include "../BWEngineCore/BWShader.h"
#include "GLUBO.h"
#include "../BWEngineCore/BWVertexIndexData.h"
#include "GLVAO.h"
class GLSLGpuProgram : public BWHighLevelGpuProgram
{
public:
	friend class GLVAO;
	GLSLGpuProgram(std::string name , std::string groupName , BWResourceManager* creator);
	~GLSLGpuProgram ();  
	void InitNamedConstantes();
	void LoadFromSource();
	bool Compile(bool checkError  =  true);
	size_t calculateSize() { return 0; }
	bool getPassTransformStates(){ return true; }
	virtual bool SetParameter(const std::string &name, void *value) override;
	virtual void Load() override;
	void bind();
	void bindingBuffer(IndexDataPrt indexData, VertexDataPrt vertexData);
	const NameLocation& getFragmentOutNameLocation() const;
	GLuint getID() { return mID; }
	void SetGPUProgramParameters(BWGpuProgramParametersPtr ProgramParameters) override;
	// 暂时测试使用
	NameLocation mNameLoaction;
protected: 
	void prepareImpl(){ }
	void preLoadImpl();
	void loadImpl();
	void postLoadImpl() { }
	void mergeNamedConstants(GpuNamedConstantsPtr namedConstants);
	void mergeUBOInforMap(const UBOInforMap &UBOMap);
	void setUniform(const GpuConstantDefinition &def , void *data);
	virtual void LoadHighLevelImp();
private:
	
	static void GetNamedConstantesFromSource(std::string &source, GpuNamedConstantsPtr namedConstants);
	
	typedef std::vector<GLSLGpuProgram*> GLSLProgramContainer;
	GLSLProgramContainer glslContainer; // attach
	GLuint handle;
	GLint complied; // = 1;
	bool  mIsCreated;  // mID是否建立
	bool  mUniformIsReload; // 是否重新建立uniform

	GLuint mID;
	UBOMap mUBOMap;
	NameLocation mFragmentOutNameLocation;
	VertexDataPrt mVertexData;
	IndexDataPrt  mIndexData;
	GLVAO *mVao;
};

#endif