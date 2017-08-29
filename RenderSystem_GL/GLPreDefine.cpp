#include "GLPreDefine.h"
#include "../BWEngineCore/BWHardwareVertexBuffer.h"
StringToEnumMap mTypeEnumMap;
BufferElementToString mBufferElementToString;
StringToBufferElement  mStringToBufferElement;
void InitMap()
{
	if (mTypeEnumMap.size() != 0)
	{
		return;
	}
	mTypeEnumMap.insert(StringToEnumMap::value_type("float", GL_FLOAT));
	mTypeEnumMap.insert(StringToEnumMap::value_type("vec2", GL_FLOAT_VEC2));
	mTypeEnumMap.insert(StringToEnumMap::value_type("vec3", GL_FLOAT_VEC3));
	mTypeEnumMap.insert(StringToEnumMap::value_type("vec4", GL_FLOAT_VEC4));
	mTypeEnumMap.insert(StringToEnumMap::value_type("sampler1D", GL_SAMPLER_1D));
	mTypeEnumMap.insert(StringToEnumMap::value_type("sampler2D", GL_SAMPLER_2D));
	mTypeEnumMap.insert(StringToEnumMap::value_type("sampler3D", GL_SAMPLER_3D));
	mTypeEnumMap.insert(StringToEnumMap::value_type("sampler2DArray", GL_SAMPLER_2D_ARRAY_EXT));
	mTypeEnumMap.insert(StringToEnumMap::value_type("samplerCube", GL_SAMPLER_CUBE));
	mTypeEnumMap.insert(StringToEnumMap::value_type("sampler1DShadow", GL_SAMPLER_1D_SHADOW));
	mTypeEnumMap.insert(StringToEnumMap::value_type("sampler2DShadow", GL_SAMPLER_2D_SHADOW));
	mTypeEnumMap.insert(StringToEnumMap::value_type("int", GL_INT));
	mTypeEnumMap.insert(StringToEnumMap::value_type("ivec2", GL_INT_VEC2));
	mTypeEnumMap.insert(StringToEnumMap::value_type("ivec3", GL_INT_VEC3));
	mTypeEnumMap.insert(StringToEnumMap::value_type("ivec4", GL_INT_VEC4));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat2", GL_FLOAT_MAT2));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat3", GL_FLOAT_MAT3));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat4", GL_FLOAT_MAT4));
	// GL 2.1
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat2x2", GL_FLOAT_MAT2));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat3x3", GL_FLOAT_MAT3));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat4x4", GL_FLOAT_MAT4));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat2x3", GL_FLOAT_MAT2x3));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat3x2", GL_FLOAT_MAT3x2));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat3x4", GL_FLOAT_MAT3x4));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat4x3", GL_FLOAT_MAT4x3));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat2x4", GL_FLOAT_MAT2x4));
	mTypeEnumMap.insert(StringToEnumMap::value_type("mat4x2", GL_FLOAT_MAT4x2));

	glewInit();
	const GLubyte* byteGlVersion = glGetString(GL_VERSION);
	const GLubyte* byteGlVendor = glGetString(GL_VENDOR);
	const GLubyte* byteGlRenderer = glGetString(GL_RENDERER);
	const GLubyte* byteSLVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::string strTemp = "OpenGL version: ";
	strTemp += (const char*)byteGlVersion;
	strTemp += "\n";
	strTemp += "GL_VENDOR: ";
	strTemp += (const char*)byteGlVendor;
	strTemp += "\n";
	strTemp += "GL_RENDERER: ";
	strTemp += (const char*)byteGlRenderer;
	strTemp += "\n";
	strTemp += "GLSL version: ";
	strTemp += (const char*)byteSLVersion;
	Log::GetInstance()->logMessage(strTemp, false);


	mStringToBufferElement.insert(StringToBufferElement::value_type("Position", VES_POSITION));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Blend_Weights", VES_BLEND_WEIGHTS));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Blend_Indices", VES_BLEND_INDICES));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Normal", VES_NORMAL));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Diffuse", VES_DIFFUSE));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Specular", VES_SPECULAR));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Texture_Coordinates", VES_TEXTURE_COORDINATES));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Binormal", VES_BINORMAL));
	mStringToBufferElement.insert(StringToBufferElement::value_type("Tangent", VES_TANGENT));


	mBufferElementToString.insert(BufferElementToString::value_type(VES_POSITION, "Position"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_BLEND_WEIGHTS, "Blend_Weights"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_BLEND_INDICES, "Blend_Indices"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_NORMAL, "Normal"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_DIFFUSE, "Diffuse"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_SPECULAR, "Specular"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_TEXTURE_COORDINATES, "Texture_Coordinates"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_BINORMAL, "Binormal"));
	mBufferElementToString.insert(BufferElementToString::value_type(VES_TANGENT, "Tangent"));
}
GLint CompleteConstantDefine(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
	    return GCT_FLOAT1;
	case GL_FLOAT_VEC2: 
		return GCT_FLOAT2;
	case GL_FLOAT_VEC3:
		return GCT_FLOAT3;
	case GL_FLOAT_VEC4:
		return GCT_FLOAT4;
	case GL_SAMPLER_1D:
		// need to record samplers for GLSL
		return GCT_SAMPLER1D;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_2D_RECT_ARB:
		return GCT_SAMPLER2D;
	case GL_SAMPLER_2D_ARRAY_EXT:
		return GCT_SAMPLER2DARRAY;
	case GL_SAMPLER_3D:
		return GCT_SAMPLER3D;
	case GL_SAMPLER_CUBE:
		return GCT_SAMPLERCUBE;
	case GL_SAMPLER_1D_SHADOW:
		return GCT_SAMPLER1DSHADOW;
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		return GCT_SAMPLER2DSHADOW;
	case GL_INT:
		return GCT_INT1;
	case GL_INT_VEC2:
		return GCT_INT2;
	case GL_INT_VEC3:
		return GCT_INT3;
	case GL_INT_VEC4:
		return GCT_INT4;
	case GL_FLOAT_MAT2:
		return GCT_MATRIX_2X2;
	case GL_FLOAT_MAT3:
		return GCT_MATRIX_3X3;
	case GL_FLOAT_MAT4:
		return GCT_MATRIX_4X4;
	case GL_FLOAT_MAT2x3:
		return GCT_MATRIX_2X3;
	case GL_FLOAT_MAT3x2:
		return GCT_MATRIX_3X2;
	case GL_FLOAT_MAT2x4:
		return GCT_MATRIX_2X4;
	case GL_FLOAT_MAT4x2:
		return GCT_MATRIX_4X2;
	case GL_FLOAT_MAT3x4:
		return GCT_MATRIX_3X4;
	case GL_FLOAT_MAT4x3:
		return GCT_MATRIX_4X3;
	default:
		return GCT_UNKNOWN;
	}
	//def.elementSize = GpuConstantDefinition::getElementSize(def.constType, false);
}
void CompleteConstantDefine(GLenum type, GpuConstantDefinition& def)
{
	switch (type)
	{
	case GL_FLOAT:
		def.constType = GCT_FLOAT1;
		break;
	case GL_FLOAT_VEC2:
		def.constType = GCT_FLOAT2;
		break;
	case GL_FLOAT_VEC3:
		def.constType = GCT_FLOAT3;
		break;
	case GL_FLOAT_VEC4:
		def.constType = GCT_FLOAT4;
		break;
	case GL_SAMPLER_1D:
		// need to record samplers for GLSL
		def.constType = GCT_SAMPLER1D;
		break;
	case GL_SAMPLER_2D:
	case GL_SAMPLER_2D_RECT_ARB:
		def.constType = GCT_SAMPLER2D;
		break;
	case GL_SAMPLER_2D_ARRAY_EXT:
		def.constType = GCT_SAMPLER2DARRAY;
		break;
	case GL_SAMPLER_3D:
		def.constType = GCT_SAMPLER3D;
		break;
	case GL_SAMPLER_CUBE:
		def.constType = GCT_SAMPLERCUBE;
		break;
	case GL_SAMPLER_1D_SHADOW:
		def.constType = GCT_SAMPLER1DSHADOW;
		break;
	case GL_SAMPLER_2D_SHADOW:
	case GL_SAMPLER_2D_RECT_SHADOW_ARB:
		def.constType = GCT_SAMPLER2DSHADOW;
		break;
	case GL_INT:
		def.constType = GCT_INT1;
		break;
	case GL_INT_VEC2:
		def.constType = GCT_INT2;
		break;
	case GL_INT_VEC3:
		def.constType = GCT_INT3;
		break;
	case GL_INT_VEC4:
		def.constType = GCT_INT4;
		break;
	case GL_FLOAT_MAT2:
		def.constType = GCT_MATRIX_2X2;
		break;
	case GL_FLOAT_MAT3:
		def.constType = GCT_MATRIX_3X3;
		break;
	case GL_FLOAT_MAT4:
		def.constType = GCT_MATRIX_4X4;
		break;
	case GL_FLOAT_MAT2x3:
		def.constType = GCT_MATRIX_2X3;
		break;
	case GL_FLOAT_MAT3x2:
		def.constType = GCT_MATRIX_3X2;
		break;
	case GL_FLOAT_MAT2x4:
		def.constType = GCT_MATRIX_2X4;
		break;
	case GL_FLOAT_MAT4x2:
		def.constType = GCT_MATRIX_4X2;
		break;
	case GL_FLOAT_MAT3x4:
		def.constType = GCT_MATRIX_3X4;
		break;
	case GL_FLOAT_MAT4x3:
		def.constType = GCT_MATRIX_4X3;
		break;
	default:
		def.constType = GCT_UNKNOWN;
		break;

	}
	def.elementSize = GpuConstantDefinition::getElementSize(def.constType, false);

}
size_t GLTypeSize(GLenum type)
{
	switch (type)
	{
	case GL_FLOAT:
		return sizeof(GLfloat);
	case GL_FLOAT_VEC2:
		return sizeof(GLfloat) * 2;
	case GL_FLOAT_VEC3:
		return sizeof(GLfloat)* 3;
	case GL_FLOAT_VEC4:
		return sizeof(GLfloat)* 4;
	case GL_INT:
		return sizeof(GLint);
	case GL_INT_VEC2:
		return sizeof(GLint) * 2;
	case GL_INT_VEC3:
		return sizeof(GLint) * 3;
	case GL_INT_VEC4:
		return sizeof(GLint) * 4;
	case GL_FLOAT_MAT2:
		return sizeof(GLfloat)* 4;
	case GL_FLOAT_MAT3:
		return sizeof(GLfloat)* 9;
	case GL_FLOAT_MAT4:
		return sizeof(GLfloat)* 16;
	case GL_FLOAT_MAT2x3:
		return sizeof(GLfloat)* 6;
	case GL_FLOAT_MAT3x2:
		return sizeof(GLfloat)* 6;
	case GL_FLOAT_MAT2x4:
		return sizeof(GLfloat)* 8;
	case GL_FLOAT_MAT4x2:
		return sizeof(GLfloat)* 8;
	case GL_FLOAT_MAT3x4:
		return sizeof(GLfloat)* 12;
	case GL_FLOAT_MAT4x3:
		return sizeof(GLfloat)* 12;
	}
	return 0;
}
GLenum getGLType(GLint type)
{
	switch (type)
	{
	case VET_FLOAT1:
	case VET_FLOAT2:
	case VET_FLOAT3:
	case VET_FLOAT4:
	case VET_COLOR:
	case VET_COLOUR_ARGB:
	case VET_COLOUR_ABGR:
		return GL_FLOAT;
	case VET_SHORT1:
	case VET_SHORT2:
	case VET_SHORT3:
	case VET_SHORT4:
		return GL_SHORT;
	case VET_UBYTE4:
		return GL_BYTE;
	default:
		break;
	}
}
bool PrintProgramInfoLog(const std::string &name, GLuint glslProgram)
{
	GLsizei bufsize;
	GLchar* infoLog;
	GLsizei length;
	glGetProgramiv(glslProgram, GL_INFO_LOG_LENGTH, &bufsize);
	if (bufsize > 1) 
	{
		infoLog = new GLchar[bufsize];
		if (infoLog != NULL)
		{
			glGetProgramInfoLog(glslProgram, bufsize, &length, infoLog);
			std::string msg = "program :" + name + "  has link message.\n";
			msg += infoLog;
			Log::GetInstance()->logMessage(msg, false);
			delete[] infoLog;
			if (msg.find("error") != std::string::npos)
			{
				return false;
			}
			return true;// 有些warning  但是不至于影响运行
		}
		else
		{
			Log::GetInstance()->logMessage("printShaderInforLog() :  there is not have many memory to allocate !!", false);
			assert(0);
		}
	}
	std::string msg;
	msg = "link " + name + " is complete . \n";
	Log::GetInstance()->logMessage(msg, false);
	return true;
}
bool printShaderInforLog(std::string &shaderName ,GLuint shaderObj)
{
	GLsizei bufSize;
	GLchar *info;
	GLsizei length;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 1) 
	{
		info = new GLchar[bufSize];
		if (info != NULL)
		{
			glGetShaderInfoLog(shaderObj, bufSize, &length, info);
			std::string msg = "Shader source : " + shaderName + " has not all right.\n";
			msg += info;
			Log::GetInstance()->logMessage(msg, false);
			delete[] info;
			if (msg.find("error") != std::string::npos)
			{
				return false; 
			}
			return true; // 包含一些warning，但是不影响运行。
		}
		else
		{
			Log::GetInstance()->logMessage("printShaderInforLog() :  there is not have many memory to allocate !!", false);
			assert(0);
		}
	}
	Log::GetInstance()->logMessage(shaderName + " is complete complied.", false);
	return true;
}

