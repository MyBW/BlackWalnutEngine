#pragma once
#include "GL\include\glew.h"
#include "GLPixelUtil.h"
#include "..\BWEngineCore\BWImageTextureBuffer.h"


class GLImageTextureBuffer : public BWImageTextureBuffer
{
public:
	GLImageTextureBuffer(const std::string& name, PixelFormat Format ,Usage usage, bool systemMemory, bool isUseShadowBuffer)
		:BWImageTextureBuffer(name, Format, usage, systemMemory, isUseShadowBuffer)
	{
		glGenBuffers(1, &GLBufferID);
		glBindBuffer(GL_TEXTURE_BUFFER, GLBufferID);
		glBufferData(GL_TEXTURE_BUFFER, 0, 0, GL_STATIC_DRAW);
		glGenTextures(1, &GLTextureID);
		glBindTexture(GL_TEXTURE_BUFFER, GLTextureID);
		GLenum  GLFormat = GLPixelUtil::getGLInternalFormat(Format); 
		glTexBuffer(GL_TEXTURE_BUFFER, GLFormat, GLBufferID);
	}
	virtual ~GLImageTextureBuffer();
	void ResizeBuffer(int InSize) override;
	GLuint GetTextureID() const { return GLTextureID; }
protected:
	void* lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option) override;
	void  unLockImp() override;
private:
	GLuint GLTextureID;
	GLuint GLBufferID;
};


class GLImageTextureBufferPtr : public SmartPointer<GLImageTextureBuffer>
{
public:
	GLImageTextureBufferPtr()
	{

	}
	GLImageTextureBufferPtr(GLImageTextureBuffer* ImageTextureBuffer)
	{
		counter = 0;
		mPointer = ImageTextureBuffer;
	}
	GLImageTextureBufferPtr(const BWImageTexturebufferPtr& texture)
	{
		if (texture.IsNull() || texture.Get() == dynamic_cast<BWImageTextureBuffer*>(Get()))
		{
			return;
		}
		if (texture.GetCounterPointer())
		{
			counter = texture.GetCounterPointer();
			(*counter)++;
			mPointer = dynamic_cast<GLImageTextureBuffer*>(texture.Get());
		}
	}
	~GLImageTextureBufferPtr() { }

private:

};