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
private:
	GLuint GLTextureID;
	GLuint GLBufferID;
};