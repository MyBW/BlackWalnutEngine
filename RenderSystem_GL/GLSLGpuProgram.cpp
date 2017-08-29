#include "GLSLGpuProgram.h"
#include "..\BWEngineCore\BWStringOperation.h"
#include "..\BWEngineCore\BWStringConverter.h"
#include "..\BWEngineCore\BWLog.h"
#include "GLPreDefine.h"
#include "GLSLShader.h"
#include "GLVAOManager.h"
#include "GLRenderSystem.h"
std::string logObjectInfo(const std::string& msg, const GLhandleARB obj)
{
	std::string logMessage = msg;

	if (obj > 0)
	{
		GLint infologLength = 0;

		if (glIsProgram(obj))
			glValidateProgram(obj);

		glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);

		if (infologLength > 0)
		{
			GLint charsWritten = 0;

			GLcharARB * infoLog = new GLcharARB[infologLength];

			glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
			logMessage += std::string(infoLog);
			//assert(0);
			//LogManager::getSingleton().logMessage(logMessage);

			delete[] infoLog;
		}
	}

	return logMessage;

}
//-----------------------------------------------------------------------------
void reportGLSLError( const std::string& ogreMethod, const std::string& errorTextPrefix, const GLhandleARB obj, const bool forceInfoLog = false, const bool forceException = false)
{
	GLenum glErr;
	bool errorsFound = false;
	std::string msg = errorTextPrefix;
	glErr = glGetError();
	// get all the GL errors
	while (glErr != GL_NO_ERROR)
	{
		const char* glerrStr = (const char*)gluErrorString(glErr);
		if (glerrStr)
		{
			msg += std::string(glerrStr);
		}
		glErr = glGetError();
		errorsFound = true;
	}


	// if errors were found then put them in the Log and raise and exception
	if (errorsFound || forceInfoLog)
	{
		// if shader or program object then get the log message and send to the log manager
		msg += logObjectInfo(msg, obj);

		if (forceException)
		{
			assert(0);
			//OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, msg, ogreMethod);
		}
	}
}

//-----------------------------------------------------------------------------


void GLSLGpuProgram::InitNamedConstantes()
{
	if (!highLevelLoaded)
	{
		LoadHighLevel();
	}
	/*GetNamedConstantesFromSource(source, namedConstantes);
	GLSLProgramContainer::iterator itor = glslContainer.begin();
	while (itor != glslContainer.end())
	{
		GetNamedConstantesFromSource((*itor)->source, namedConstantes);
		itor++;
	}*/
}
void GLSLGpuProgram::LoadFromSource()
{
	if (source.empty())
	{
		return;
	}
	if (IsSupported())
	{
		reportGLSLError("GLSLProgram::LoadFromSource ", "GLSL error before  creating shader obj", 0);
		GLuint shaderType;
		switch (type)
		{
		case GPT_VERTEX_PROGRAM:
			shaderType = GL_VERTEX_SHADER;
			break;
		case GPT_FRAGMENT_PROGRAM:
			shaderType = GL_FRAGMENT_SHADER;
			break;
		case GPT_GEOMETRY_PROGRAM:
			shaderType = GL_GEOMETRY_SHADER;
			break;
		default:
			break;
		}
	   handle = glCreateShader(shaderType);
		reportGLSLError("GLSLProgram::LoadFromSource ", "GLSL error after  creating shader obj", 0);
	   Compile();
	}

}
bool GLSLGpuProgram::Compile(bool checkError)
{
	if (checkError)
	{
		logObjectInfo("glsl compiling :" + name, handle);
	}
	const GLchar *src = new GLchar[source.length()];
	memcpy(const_cast<GLchar*>(src), source.c_str(), source.length());
	GLint size = source.length();
	glShaderSource(handle, 1, &src , &size);
	delete src;
	glCompileShader(handle);
	glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &complied);
	if (checkError)
	{
		reportGLSLError("glsl compile error ", "can not compile  program :" + name, handle, !complied, !complied);
		if (complied)
		{
			logObjectInfo("glsl compiled :" + name, handle);
		}
	}
	return (complied == 1);
	
}
void GLSLGpuProgram::GetNamedConstantesFromSource(std::string &source, GpuNamedConstantsPtr namedConstants)
{
	std::size_t currPos = source.find("uniform");
	while(currPos != std::string::npos)
	{
		std::string tmp = source.substr(currPos, source.find_first_of(';' , currPos) - currPos);
		StringUtil::DeleteChar(tmp, '\t');
		std::size_t begin = tmp.find_first_not_of(" ", tmp.find("uniform") + 7);
		std::string type = tmp.substr(begin, tmp.find_first_of(" " ,begin) - begin);
		std::string name = tmp.substr(tmp.find(type) + type.length());
		StringUtil::DeleteChar(type, ' ');
		StringUtil::DeleteChar(name, ' ');
		StringToEnumMap::iterator itor = mTypeEnumMap.find(type);
		GpuConstantDefinition def;
		if (itor != mTypeEnumMap.end())
		{
			CompleteConstantDefine(itor->second, def);
		}
		else
		{
			assert(0);
		}
		std::size_t pos = name.find("[");
		if (pos != std::string::npos)
		{
			std::string num = name.substr(pos, name.find(']', pos) - pos);
			name = name.substr(0, pos);
			def.arraySize = StringConverter::ParseInt(num);
		}
		else
		{
			def.arraySize = 1;
		}
		def.logicalIndex = 0;
		if (def.isFloat())
		{
			def.physicalIndex = namedConstants->floatBufferSize;
			namedConstants->floatBufferSize += def.arraySize * def.elementSize;
		}
		else
		{
			def.physicalIndex = namedConstants->intBufferSize;
			namedConstants->intBufferSize += def.arraySize * def.elementSize;
		}
		namedConstants->map.insert(GpuConstantDefinitionMap::value_type(name, def));
		namedConstants->generateConstantDefinitionArrayEntries(name, def);
		currPos = source.find("uniform", currPos + 7);
	}
}
GLSLGpuProgram::GLSLGpuProgram(std::string name, std::string groupName, BWResourceManager* creator) :
BWHighLevelGpuProgram(name, groupName, creator), 
complied(1), 
handle(0),
mVao(NULL),
mIsCreated(false),
mUniformIsReload(true)
{
	
}
void GLSLGpuProgram::mergeNamedConstants(GpuNamedConstantsPtr src)
{
	GpuConstantDefinitionMap::iterator  itor = src->map.begin();
	while (itor != src->map.end())
	{
		if (namedConstantes->map.find(itor->first) == namedConstantes->map.end() )
		{
			GpuConstantDefinition def = itor->second;
			if (def.isFloat())
			{
				def.physicalIndex = namedConstantes->floatBufferSize;
				namedConstantes->floatBufferSize += def.arraySize * def.elementSize;
			}
			else
			{
				def.physicalIndex = namedConstantes->intBufferSize;
				namedConstantes->intBufferSize += def.arraySize * def.elementSize;
			}
			namedConstantes->map.insert(GpuConstantDefinitionMap::value_type(itor->first, def));
			if (def.arraySize != 1)
			{
				namedConstantes->generateConstantDefinitionArrayEntries(name, def);
			}
		}
		itor++;
	}
}
void GLSLGpuProgram::mergeUBOInforMap(const UBOInforMap &UBOInfoMap)
{
	UBOInforMap::const_iterator itor = UBOInfoMap.begin();
	
	while (itor != UBOInfoMap.end())
	{
		if (mUBOMap[itor->first] == NULL)
		{
			GLUniformBufferObject *ubo = new GLUniformBufferObject(itor->second, BWHardwareBuffer::HBU_DYNAMIC);
			mUBOMap[itor->first] = ubo;
		}
		itor++; 
	}
}
void GLSLGpuProgram::preLoadImpl()
{
	if (!mIsCreated)
	{
		mID = glCreateProgram();
		mIsCreated = true;
	}
	if (mUniformIsReload)
	{
		ShaderList::iterator itor = mShaderList.begin();
		while (itor != mShaderList.end())
		{
			GLSLShader *shader = dynamic_cast<GLSLShader*>(itor->Get());
			shader->Load();
			if (shader->getShaderType() == ST_VERTEX_SHADER)
			{
				// 获得VAO相关变量数据
				mNameLoaction = shader->getInConstant();
			}
			if (shader->getShaderType() == ST_FRAGMENT_SHADER)
			{
				// 获得绑定renderbuffer的相关数据
				mFragmentOutNameLocation = shader->GetFragmentOutConstant();
			}
			GpuNamedConstantsPtr namedConstants = shader->getNamedConstant();
			mergeNamedConstants(namedConstants);
			UBOInforMap inforMap = shader->getUBOInfor();
			mergeUBOInforMap(inforMap);
			CHECK_GL_ERROR(glAttachShader(mID, shader->getGLID()));
			itor++;
		}
		mUniformIsReload = false;
	}
	
} 
const NameLocation& GLSLGpuProgram::getFragmentOutNameLocation() const
{
	return mFragmentOutNameLocation;
}

void GLSLGpuProgram::SetGPUProgramParameters(BWGpuProgramParametersPtr ProgramParameters)
{
	GpuNamedConstantsPtr namedConstant = ProgramParameters->getNamedConstants();
	if (namedConstant.IsNull())
	{
		namedConstant = GetDefaultParameters()->getNamedConstants();
	}
	//设置各种unifomr参数
	GpuConstantDefinitionMap::iterator itor = namedConstant->map.begin();
	while (itor != namedConstant->map.end())
	{
		GpuConstantDefinition *def = &(itor->second);
		int size = def->arraySize * itor->second.getElementSize(def->constType, false);
		void *data = NULL;
		int index = def->physicalIndex;
		if (def->isFloat())
		{
			data = ProgramParameters->GetFloatPointer(def->physicalIndex);
		}
		else
		{
			data = ProgramParameters->GetIntPointer(def->physicalIndex);
		}
		if (itor->second.getElementSize(def->constType, false) > 4)
		{
			float* tmpdata = (float*)data;
			float finaldata[16];
			for (int i = 0; i < 16; i++)
			{
				finaldata[i] = tmpdata[i];
			}
			GLRenderSystem::GLtranspose(def->constType, finaldata);
			SetParameter(itor->first, finaldata);
		}
		else
		{
			SetParameter(itor->first, data);
		}
		itor++;
	}
}

bool GLSLGpuProgram::SetParameter(const std::string &name, void *value)
{
	UBOMap::iterator itor = mUBOMap.begin();
	while (itor != mUBOMap.end())
	{
		if (itor->second->setUniform(name, value))
		{
			return true;
		}
		itor++;
	}
	GpuConstantDefinitionMap::iterator defMapItor = namedConstantes->map.find(name);
	if (defMapItor != namedConstantes->map.end())
	{
		setUniform(defMapItor->second, value);
		return true;
	}
	return false;
}

void GLSLGpuProgram::Load()
{
	BWHighLevelGpuProgram::Load();
	bind();
}

void GLSLGpuProgram::setUniform(const GpuConstantDefinition &def , void *srcData)
{
	if (def.isFloat())
	{
		int size = def.arraySize * def.getElementSize(def.constType, false);
		GLfloat *data = new GLfloat[size];
		float *tmpData =(float *) srcData; // 不是用memcpy赋值 是因为有可能GLfloat 和float 不一致
		for (int i = 0; i < size; i++)
		{
			data[i] = tmpData[i] ;
		}
		//memcpy(data, &mFloatList[def.physicalIndex], size);
		switch (def.constType)
		{
		case GCT_FLOAT1:
			CHECK_GL_ERROR(glUniform1fv(def.logicalIndex,def.arraySize , data));
			break;
		case GCT_FLOAT2:
			glUniform2fv(def.logicalIndex, def.arraySize, data);
			break;
		case GCT_FLOAT3:
			glUniform3fv(def.logicalIndex, def.arraySize , data);
			break;
		case GCT_FLOAT4:
			CHECK_GL_ERROR(glUniform4fv(def.logicalIndex, def.arraySize, data));
			break;
		case GCT_MATRIX_2X2:
			glUniformMatrix2fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_2X3:
			glUniformMatrix2x3fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_2X4:
			glUniformMatrix2x4fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_3X2:
			glUniformMatrix3x2fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_3X3:
			glUniformMatrix3fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_3X4:
			glUniformMatrix3x4fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_4X2:
			glUniformMatrix4x2fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_4X3:
			glUniformMatrix4x3fv(def.logicalIndex, def.arraySize, false, data);
			break;
		case GCT_MATRIX_4X4:
			glUniformMatrix4fv(def.logicalIndex, def.arraySize, false, data);
			break;
		}
		delete [] data;
		return;
	}
	else
	{
		int size = def.arraySize * def.getElementSize(def.constType, false);
		GLint *data = new GLint[size];
		int *tmpData = (int *)srcData;
		for (int i = 0; i < size; i++)
		{
			data[i] = tmpData[i];
		}
		//memcpy(data, &mIntList[def.physicalIndex], def.arraySize * def.getElementSize(def.constType, false));
		switch (def.constType)
		{
		case GCT_INT1:
		case GCT_SAMPLER2D:
		case GCT_SAMPLERCUBE:
			CHECK_GL_ERROR(glUniform1iv(def.logicalIndex, def.arraySize, data));
			break; 
		case GCT_INT2:
			glUniform2iv(def.logicalIndex, def.arraySize, data);
			break;
		case GCT_INT3:
			glUniform3iv(def.logicalIndex, def.arraySize, data);
			break;
		case GCT_INT4:
			glUniform4iv(def.logicalIndex, def.arraySize, data);
			break;
		case GCT_SAMPLER1D:
		case GCT_SAMPLER2DARRAY:
		case GCT_SAMPLER3D:
		case GCT_SAMPLER1DSHADOW:
		case GCT_SAMPLER2DSHADOW:
			Log::GetInstance()->logMessage("GLSLGpuProgram::setUniform(): Dont support sampler! \n", false);
			assert(0); 
		}
		delete[] data;
		return;
	}
	Log::GetInstance()->logMessage("GLSLGpuProgram::setUniform(): wrong data ! \n", false);
	assert(0);
	/*switch (def.constType)
	{
	case GCT_FLOAT1:
		glUniform1f(def.logicalIndex, mFloatList[def.physicalIndex]);
		break;
	case GCT_FLOAT2:
		glUniform2f(def.logicalIndex, mFloatList[def.physicalIndex], mFloatList[def.physicalIndex + 1]);
		break;
	case GCT_FLOAT3:
		glUniform3f(def.logicalIndex, mFloatList[def.physicalIndex], mFloatList[def.physicalIndex + 1], mFloatList[def.physicalIndex + 2]);
		break;
	case GCT_FLOAT4:
		break;
	case GCT_SAMPLER1D:
		break;
	case GCT_SAMPLER2D:
		break;
	case GCT_SAMPLER3D:
		break;
	case GCT_SAMPLERCUBE:
		break;
	case GCT_SAMPLER1DSHADOW:
		break;
	case GCT_SAMPLER2DSHADOW:
		break;
	case GCT_SAMPLER2DARRAY:
		break;
	case GCT_MATRIX_2X2:
		break;
	case GCT_MATRIX_2X3:
		break;
	case GCT_MATRIX_2X4:
		break;
	case GCT_MATRIX_3X2:
		break;
	case GCT_MATRIX_3X3:
		break;
	case GCT_MATRIX_3X4:
		break;
	case GCT_MATRIX_4X2:
		break;
	case GCT_MATRIX_4X3:
		break;
	case GCT_MATRIX_4X4:
		break;
	case GCT_INT1:
		break;
	case GCT_INT2:
		break;
	case GCT_INT3:
		break;
	case GCT_INT4:
		break;
	case GCT_UNKNOWN:
		break;
	default:
		break;
	}*/
}
void GLSLGpuProgram::bind()
{
	if (mLoadState != LOADSTATE_LOADED)
	{
		Load();
	}
	CHECK_GL_ERROR(glUseProgram(mID));
	UBOMap::iterator itor = mUBOMap.begin();
	while (itor != mUBOMap.end())
	{
		itor->second->bind();
		itor++;
	}
}

void GLSLGpuProgram::bindingBuffer(IndexDataPrt indexData, VertexDataPrt vertexData)
{
	mVao = GLVAOManager::GetInstance()->createVAO(this, indexData, vertexData);
	if (mVao)
	{
		mVao->bind();
	}
}
void GLSLGpuProgram::loadImpl()
{
	if (!mLinkeFinish)
	{
		CHECK_GL_ERROR(glLinkProgram(mID));
		mLinkeFinish = PrintProgramInfoLog(name, mID);
		if (!mLinkeFinish)
		{
			return;
		}
		// 设置各种uniform的location
		GpuConstantDefinitionMap::iterator defMapItor = namedConstantes->map.begin();
		while (defMapItor != namedConstantes->map.end())
		{
			defMapItor->second.logicalIndex = glGetUniformLocation(mID, defMapItor->first.c_str());
			defMapItor++;
		}
		
		mFloatList.resize(namedConstantes->floatBufferSize);
		mIntList.resize(namedConstantes->intBufferSize);
		// 设置UBO
		UBOMap::iterator  ubo = mUBOMap.begin();
		static GLint binding = 0; // UBO的绑定序号要求是全局唯一的 否则出现错误
		while (ubo != mUBOMap.end())
		{
			if (ubo->second->getBindPoint() ==-1)
			{
				ubo->second->setBindPoint(binding);
				binding++;
			}
			else
			{
				binding = ubo->second->getBindPoint();
			}
			ubo->second->initUBO(mID);
			ubo++;
		}
		NameLocation::iterator  itor = mNameLoaction.begin();
		while (itor != mNameLoaction.end())
		{
			itor->second = glGetAttribLocation(mID, itor->first.c_str());
			itor++;
		}
	}
}
void GLSLGpuProgram::LoadHighLevelImp()
{
	preLoadImpl();
}
GLSLGpuProgram::~GLSLGpuProgram()
{
	GLVAOManager::GetInstance()->releaseVAO(this);
}

