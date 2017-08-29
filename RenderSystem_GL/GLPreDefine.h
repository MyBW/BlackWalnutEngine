#ifndef GLPREDEFINE_H_
#define GLPREDEFINE_H_



#include "../BWEngineCore/BWLog.h"
#include "GL/include/glew.h"
#include "../BWEngineCore/BWGpuProgramParams.h"
#include "../BWEngineCore/BWStringConverter.h"
#define CHECK_GL_ERROR(glfunc) \
{\
	glfunc; \
	int e = glGetError(); \
if (e)\
	{\
	const char *errorStr = ""; \
	switch (e) \
		{\
	case GL_INVALID_VALUE:    errorStr = "GL_INVALID_VALUE"; break; \
	case GL_INVALID_ENUM:     errorStr = "GL_INVALID_ENUM"; break; \
	case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break; \
	case GL_INVALID_FRAMEBUFFER_OPERATION: errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break; \
	case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMEORY"; break; \
	default: break; \
		}\
		std::string msgBuf; \
		msgBuf += "OpenGL  Error:	"; \
		msgBuf += errorStr; \
		msgBuf += "\n File:"; \
		msgBuf += __FILE__; \
		msgBuf += "\n Line:"; \
		msgBuf += StringConverter::ToString(__LINE__); \
		Log::GetInstance()->logMessage(msgBuf, false); \
	}\
}


bool printShaderInforLog(std::string &shaderName, GLuint shaderObj);
bool PrintProgramInfoLog(const std::string &name, GLuint glslProgram);
void InitMap();
void CompleteConstantDefine(GLenum type, GpuConstantDefinition& def);
GLint CompleteConstantDefine(GLenum type);
size_t GLTypeSize(GLenum type);
GLenum getGLType(GLint type);
typedef std::map<std::string, GLenum> StringToEnumMap;
extern StringToEnumMap mTypeEnumMap;

typedef std::map<std::string, GLint> StringToBufferElement;
extern StringToBufferElement  mStringToBufferElement;

typedef std::map<GLint, std::string> BufferElementToString;
extern BufferElementToString mBufferElementToString;
#endif
