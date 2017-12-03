#pragma once
#include "..\BWEngineCore\BWUniformBufferObject.h"
#include "GL\include\glew.h"
#include "GLPreDefine.h"

class NewGLUniformBufferObject : public BWUniformBufferObject
{
public:
	NewGLUniformBufferObject(const std::string &Name) :BWUniformBufferObject(Name)
	{
		CHECK_GL_ERROR(glGenBuffers(1, &BufferID));
	}
	void UploadData(char* Content, int Size) override;
	GLuint GetBufferID() { return BufferID; }
	~NewGLUniformBufferObject()
	{
		glDeleteBuffers(1, &BufferID);
	}
protected:
	void *lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void unLockImp();
	GLuint BufferID;
};
