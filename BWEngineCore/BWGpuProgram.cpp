#include "BWGpuProgram.h"
#include "BWDataStream.h"
#include "BWStringConverter.h"
#include "BWGpuProgramManager.h"
BWGpuProgram::BWGpuProgram()
{
	/*if (CreateStringInterface("BWGpuProgram"))
	{
		SetBaseParamDirection();
	}*/
}
BWGpuProgram::~BWGpuProgram()
{

}
void BWGpuProgram::SetBaseParamDirection()
{
	/*ParamCommandMap* cmdMap = GetStringInterface();
	cmdMap->insert(ParamCommandMap::value_type("type", &cmdtype));
	cmdMap->insert(ParamCommandMap::value_type("syntax", &cmdsyntax));
	cmdMap->insert(ParamCommandMap::value_type("includes_skeletal_animation", &cmdskeletal));
	cmdMap->insert(ParamCommandMap::value_type("includes_morph_animation", &cmdmorph));
	cmdMap->insert(ParamCommandMap::value_type("includes_pose_animation", &cmdpose));
	cmdMap->insert(ParamCommandMap::value_type("uses_vertex_texture_fetch", &cmdVTF));
	cmdMap->insert(ParamCommandMap::value_type("uses_adjacency_information", &cmdadjaceny));
	cmdMap->insert(ParamCommandMap::value_type("manual_named_constants", &cmdmanualNameConstsFile));*/
}


////-----------------------------------------------------------------------
// std::string BWGpuProgram::CmdType::DoGet(const void* target) const
//{
//	 const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	if (t->GetType() == GPT_VERTEX_PROGRAM)
//	{
//		return "vertex_program";
//	}
//	else if (t->GetType() == GPT_GEOMETRY_PROGRAM)
//	{
//		return "geometry_program";
//	}
//	else
//	{
//		return "fragment_program";
//	}
//}
//void BWGpuProgram::CmdType::DoSet(void* target, const std::string& val) const 
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	if (val == "vertex_program")
//	{
//		t->SetType(GPT_VERTEX_PROGRAM);
//	}
//	else if (val == "geometry_program")
//	{
//		t->SetType(GPT_GEOMETRY_PROGRAM);
//	}
//	else
//	{
//		t->SetType(GPT_FRAGMENT_PROGRAM);
//	}
//}
////-----------------------------------------------------------------------
// std::string BWGpuProgram::CmdSyntax::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return t->GetSyntaxCode();
//}
//void BWGpuProgram::CmdSyntax::DoSet(void* target, const std::string& val) const 
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetSyntaxCode(val);
//}
////-----------------------------------------------------------------------
// std::string BWGpuProgram::CmdSkeletal::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return StringConverter::ToString(t->IsSkeletalAnimationIncluded());
//}
//void BWGpuProgram::CmdSkeletal::DoSet(void* target, const std::string& val) const 
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetSkeletalAnimationIncluded(StringConverter::ParseBool(val));
//}
////-----------------------------------------------------------------------
//std::string BWGpuProgram::CmdMorph::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return StringConverter::ToString(t->IsMorphAnimationIncluded());
//}
//void BWGpuProgram::CmdMorph::DoSet(void* target, const std::string& val) const
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetMorphAnimationIncluded(StringConverter::ParseBool(val));
//}
////-----------------------------------------------------------------------
//std::string BWGpuProgram::CmdPose::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return StringConverter::ToString(t->GetNumberOfPosesIncluded());
//}
//void BWGpuProgram::CmdPose::DoSet(void* target, const std::string& val) const 
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetPoseAnimationIncluded((unsigned short)StringConverter::ParseUnsignedInt(val));
//}
////-----------------------------------------------------------------------
//std::string BWGpuProgram::CmdVTF::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return StringConverter::ToString(t->IsVertexTextureFetchRequired());
//}
//void BWGpuProgram::CmdVTF::DoSet(void* target, const std::string& val) const 
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetVertexTextureFetchRequired(StringConverter::ParseBool(val));
//}
////-----------------------------------------------------------------------
//std::string BWGpuProgram::CmdManualNameConstsFile::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return t->GetManualNamedConstantsFile();
//}
//void BWGpuProgram::CmdManualNameConstsFile::DoSet(void* target, const std::string& val) const 
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetManualNamedConstantsFile(val);
//}
////-----------------------------------------------------------------------
//std::string BWGpuProgram::CmdAdjacency::DoGet(const void* target) const
//{
//	const BWGpuProgram* t = static_cast<const BWGpuProgram*>(target);
//	return StringConverter::ToString(t->IsAdjacencyInfoRequired());
//}
//void BWGpuProgram::CmdAdjacency::DoSet(void* target, const std::string& val) const
//{
//	BWGpuProgram* t = static_cast<BWGpuProgram*>(target);
//	t->SetAdjacencyInfoRequired(StringConverter::ParseBool(val));
//}

bool BWGpuProgram::IsSupported()
{
	return true;
}

void BWGpuProgram::SetMorphAnimationIncluded(bool isInclude)
{
	morphAnimationIncluded = isInclude;
}

void BWGpuProgram::SetPoseAnimationIncluded(int isInclude)
{
	poseAnimationIncluded = isInclude;
}

void BWGpuProgram::SetSkeletalAnimationIncluded(bool isInclude)
{
	skeletalAnimationIncluded = isInclude;
}

void BWGpuProgram::SetVertexTextureFetchRequired(bool isRequird)
{
	vertexTextureFetchRequired = isRequird;
}
void BWGpuProgram::SetSourceFile(const std::string & sourceFile)
{
	this->sourcefile = sourceFile;
	source.clear();
	isLoadFileName = true;
	isCompilerError = false;
}

void BWGpuProgram::SetSource(const std::string & source)
{
	this->source = source;
	sourcefile.clear();
	isLoadFileName = false;
	isCompilerError = false;
}

void BWGpuProgram::SetSyntaxCode(const std::string &language)
{
	syntaxCode = language;
}

void BWGpuProgram::SetManualNamedConstantsFile(const std::string &file)
{
	manualNameConstantsFile = file;
}

void BWGpuProgram::SetAdjacencyInfoRequired(bool isRequired)
{
	adjacencyInfoRequired = isRequired;
}

void BWGpuProgram::SetType(GpuProgramType type)
{
	this->type = type;
}

const std::string BWGpuProgram::GetSyntaxCode() const
{
	return syntaxCode;
}

GpuProgramType BWGpuProgram::GetType() const
{
	return type;
}

bool BWGpuProgram::IsSkeletalAnimationIncluded() const
{
	return skeletalAnimationIncluded;
}

bool BWGpuProgram::IsMorphAnimationIncluded() const
{
	return morphAnimationIncluded;
}

bool BWGpuProgram::IsVertexTextureFetchRequired() const
{
	return vertexTextureFetchRequired;
}

bool BWGpuProgram::IsAdjacencyInfoRequired() const
{
	return adjacencyInfoRequired;
}
bool BWGpuProgram::getPassFogStates() const
{
	assert(0);
	return true;
}
bool BWGpuProgram::getPassSurfaceAndLightStates() const
{
	return true;
}
int BWGpuProgram::GetNumberOfPosesIncluded() const
{
	return poseAnimationIncluded;
}

std::string BWGpuProgram::GetManualNamedConstantsFile() const
{
	return manualNameConstantsFile;
}

BWGpuProgramParametersPtr BWGpuProgram::GetDefaultParameters()
{
	if (defaultPararmeter.IsNull())
	{
		defaultPararmeter = CreateParameter();
	}
	return defaultPararmeter;
}

BWGpuProgramParametersPtr BWGpuProgram::CreateParameter()
{
	BWGpuProgramParametersPtr ret = BWGpuProgramManager::GetInstance()->CreateParameters();
	if (!manualNameConstantsFile.empty() && !loadManualNameConstant)
	{
		GpuNamedConstants namedConstants;
		BWDataStreamPrt data = BWResourceGroupManager::GetInstance()->OpenResource(manualNameConstantsFile, GetGroupName());
		namedConstants.load(data);
		this->namedConstantes = &namedConstants;
		loadManualNameConstant = true;
	}
	if (!namedConstantes.IsNull() && !namedConstantes->map.empty())
	{
		ret->SetNamedConstants(namedConstantes);
	}
	ret->SetLogicalIndexes(floatLogicalToPhysical, intLogicalToPhysical);
	if (!defaultPararmeter.IsNull())
	{
		ret->CopyConstantsFrom(*(defaultPararmeter.Get()));
	}
	return ret;
}  

void BWGpuProgram::CreateParameterMappingStructure(bool recreatIfExists) const
{
	assert(0);
}

