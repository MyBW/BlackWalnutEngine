#ifndef GPUPROGRAM_H_
#define GPUPROGRAM_H_
#include <vector>
#include "AllSmartPointRef.h"
#include "BWResource.h"
#include "GPUProgramType.h"
class BWGpuProgram : public BWResource
{
public:
	BWGpuProgram();
	BWGpuProgram(const std::string &name, const std::string &groupName, BWResourceManager* creator);
	virtual ~BWGpuProgram();
	//返回当前的渲染系统和硬件是否支持该program
	bool IsSupported();
	void SetMorphAnimationIncluded(bool);
	void SetPoseAnimationIncluded(int);
	void SetSkeletalAnimationIncluded(bool);
	void SetVertexTextureFetchRequired(bool);
	void SetSourceFile(const std::string & sourceFileName); // 文件名称
	void SetSource(const std::string & source);  //程序源文件
	void SetSyntaxCode(const std::string &language);
	void SetManualNamedConstantsFile(const std::string &file);
	void SetAdjacencyInfoRequired(bool);
	void SetType(GpuProgramType type);
	void SetBaseParamDirection();

	const std::string GetSyntaxCode() const;
	GpuProgramType GetType() const ;
	BWGpuProgramParametersPtr GetDefaultParameters();
	bool IsSkeletalAnimationIncluded() const ;
	bool IsMorphAnimationIncluded() const;
	bool IsVertexTextureFetchRequired() const;
	bool IsAdjacencyInfoRequired() const;
	int GetNumberOfPosesIncluded() const ;
	bool getPassSurfaceAndLightStates() const;
	bool getPassFogStates() const;
	std::string GetManualNamedConstantsFile() const;
	virtual BWGpuProgramParametersPtr CreateParameter();
	virtual void CreateParameterMappingStructure(bool recreatIfExists) const;
	virtual bool getPassTransformStates() const { return false; }
	

protected:
	void prepareImpl(){ }
	void preLoadImpl(){ }
	void loadImpl(){ }
	void postLoadImpl() { }
	/*class CmdType : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target)  const;
		void DoSet(void *target, const std::string& value) const;
	};
	class CmdSyntax : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};
	class CmdSkeletal : public ParamCommand
	{
	public: 
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};
	class CmdMorph : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};
	
	class CmdPose : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};

	class CmdVTF : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};

	class CmdManualNameConstsFile : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};

	class CmdAdjacency : public ParamCommand
	{
	public:
		 std::string DoGet(const void *target) const;
		void DoSet(void *target, const std::string& value) const;
	};
*/
	/*static CmdAdjacency cmdadjaceny;
	static CmdManualNameConstsFile cmdmanualNameConstsFile;
	static CmdMorph cmdmorph;
	static CmdPose cmdpose;
	static CmdSkeletal cmdskeletal;
	static CmdSyntax cmdsyntax;
	static CmdType cmdtype;
	static CmdVTF cmdVTF;*/
	BWGpuProgramParametersPtr defaultPararmeter;
	std::string sourcefile;
	std::string source;
	bool isLoadFileName;
	bool isCompilerError;
	bool isInitNamedConstantes;

	GpuNamedConstantsPtr namedConstantes;
	
	std::vector<int> mIntList;
	std::vector<float> mFloatList;
	GpuLogicalBufferStructPtr floatLogicalToPhysical;
	GpuLogicalBufferStructPtr intLogicalToPhysical;
    
	GpuProgramType type;
	
private:
	bool morphAnimationIncluded;
	int  poseAnimationIncluded ;
	bool skeletalAnimationIncluded;
	bool vertexTextureFetchRequired;
	bool adjacencyInfoRequired;
	

	
	std::string syntaxCode;

	std::string manualNameConstantsFile;
	

	bool loadManualNameConstant;
};
#endif