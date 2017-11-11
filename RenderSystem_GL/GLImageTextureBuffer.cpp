#include "GLImageTextureBuffer.h"

void GLImageTextureBuffer::ResizeBuffer(int InSize)
{
	glBindBuffer(GL_TEXTURE_BUFFER, GLBufferID);
	glBufferData(GL_TEXTURE_BUFFER, InSize, 0, GL_STATIC_DRAW);
}

GLImageTextureBuffer::~GLImageTextureBuffer()
{
	glDeleteBuffers(1, &GLBufferID);
	glDeleteTextures(1, &GLTextureID);
}
