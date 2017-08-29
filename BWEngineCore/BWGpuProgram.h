#ifndef GPUPROGRAM_H_
#define GPUPROGRAM_H_

#include "BWSmartPointer.h"
#include "BWGpuProgramParams.h"
#include "BWResource.h"
enum GpuProgramType
{
	GPT_VERTEX_PROGRAM,
	GPT_FRAGMENT_PROGRAM,
	GPT_GEOMETRY_PROGRAM,
	GPT_GPU_PROGRAM
};
class BWGpuProgram;
class BWGpuProgram : public BWResource
{
public:
	BWGpuProgram();
	BWGpuProgram(const std::string &name, const std::string &groupName, BWResourceManager* creator) : BWResource(creator, name, groupName), isInitNamedConstantes(false), namedConstantes(new GpuNamedConstants)
	{

	}
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
	BWGpuProgramParameters::IntList mIntList;
	BWGpuProgramParameters::FloatList mFloatList;
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



class BWGpuProgramPtr : public SmartPointer<BWGpuProgram>
{
public:
	BWGpuProgramPtr() :SmartPointer<BWGpuProgram>(){}
	BWGpuProgramPtr(BWGpuProgram* ptr) :SmartPointer<BWGpuProgram>(ptr){}
	BWGpuProgramPtr(const BWResourcePtr& resource)
	{
		mPointer = dynamic_cast<BWGpuProgram*>(resource.Get());
		counter = resource.GetCounterPointer();
		(*counter)++;

	}
	const BWGpuProgramPtr& operator=(BWResourcePtr resource)
	{
		if (mPointer == dynamic_cast<BWGpuProgram*> (resource.Get()))
		{
			return *this;
		}
		if (mPointer)
		{
			(*counter)--;
			if ((*counter) == 0)
			{
				delete mPointer;
			}
		}
		mPointer = dynamic_cast<BWGpuProgram*>(resource.Get());
		counter = resource.GetCounterPointer();
	}
};

#endif