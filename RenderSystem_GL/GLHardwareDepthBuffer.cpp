
#include "GLHardwareDepthBuffer.h"
#include "../BWEngineCore/BWRenderTarget.h"
#include "GLRenderTarget.h"
#include "../BWEngineCore/BWLog.h"
#include "GLPreDefine.h"
GLHardwareDepthBuffer::GLHardwareDepthBuffer(const std::string &name, size_t width, size_t height, size_t depth,
	BWRenderTarget* renderTarget, BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareDepthBuffer(width, height, depth, usage, useSystemMemory, useShadowBuffer)
{

	mName = name;
	glGenTextures(1, &mDepthBufferID);
	glBindTexture(GL_TEXTURE_2D, mDepthBufferID);
	if (IsWithStencil)
	{
		mInternalFormat = GL_DEPTH32F_STENCIL8;
		mAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
		
	}
	else
	{
		mInternalFormat = GL_DEPTH_COMPONENT32F;
		mAttachment = GL_DEPTH_ATTACHMENT;
		
	}
	//防止人工痕迹
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLHardwareDepthBuffer::~GLHardwareDepthBuffer()
{
	//glDeleteRenderbuffers(1, &mDepthBufferID);
	glDeleteTextures(1, &mDepthBufferID);
}

bool GLHardwareDepthBuffer::attachToRenderTarget(BWRenderTarget* renderTarget)
{
	if (renderTarget == mRenderTarget)
	{
		return true;
	}
	if (!renderTarget) return false;
	GLint CurrentFrameBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
	mRenderTarget = renderTarget;
	GLRenderTarget *frameBuffer = dynamic_cast<GLRenderTarget*>(renderTarget);
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getID()));
	glFramebufferTexture2D(GL_FRAMEBUFFER, mAttachment, GL_TEXTURE_2D, mDepthBufferID, 0);
	glBindTexture(GL_TEXTURE_2D, mDepthBufferID);
	if (IsWithStencil)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (statues != GL_FRAMEBUFFER_COMPLETE)//某个绑定到attachment上的元素无效
	{
		Log::GetInstance()->logMessage("GLRenderBuffer::attachToRenderTarget() : Error");
		return false;
	}
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, CurrentFrameBuffer));
	return true;
}

void GLHardwareDepthBuffer::RemoveFromRenderTarget()
{
	if (mRenderTarget)
	{
		GLint CurrentFrameBuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
		GLRenderTarget *frameBuffer = dynamic_cast<GLRenderTarget*>(mRenderTarget);
		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getID()));
		glFramebufferTexture2D(GL_FRAMEBUFFER, mAttachment, GL_TEXTURE_2D, 0, 0);
		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, CurrentFrameBuffer));
		mRenderTarget = NULL;
	}
}

void GLHardwareDepthBuffer::SetIsWithStencil(bool IsWithStencil)
{
	if (this->IsWithStencil != IsWithStencil)
	{
		this->IsWithStencil = IsWithStencil;
		glBindTexture(GL_TEXTURE_2D, mDepthBufferID);
		if (IsWithStencil)
		{
			mInternalFormat = GL_DEPTH32F_STENCIL8;
			mAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
			glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, 1024, 768, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
		}
		else
		{
			mInternalFormat = GL_DEPTH_COMPONENT32F;
			mAttachment = GL_DEPTH_ATTACHMENT;
			glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			//防止人工痕迹
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		}
	}
}
void* GLHardwareDepthBuffer::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	return NULL;
}
void  GLHardwareDepthBuffer::unLockImp()
{
}

void GLHardwareDepthBuffer::SetBufferSizeImp(int Width, int Heigh)
{
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, mDepthBufferID));
	if (IsWithStencil)
	{
		CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL));
	}
	else
	{
		CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	}
}
