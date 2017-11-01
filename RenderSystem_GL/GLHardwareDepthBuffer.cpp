
#include "GLHardwareDepthBuffer.h"
#include "../BWEngineCore/BWRenderTarget.h"
#include "GLRenderTarget.h"
#include "../BWEngineCore/BWLog.h"
#include "GLPreDefine.h"
#include "GLPixelUtil.h"
GLHardwareDepthBuffer::GLHardwareDepthBuffer(const std::string &name, size_t width, size_t height, size_t depth,
	BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareDepthBuffer(name ,width, height, depth, usage, useSystemMemory, useShadowBuffer)
{
	glGenTextures(1, &mDepthBufferID);
	glBindTexture(GL_TEXTURE_2D, mDepthBufferID);
	if (IsWithStencil)
	{
		mInternalFormat = GL_DEPTH32F_STENCIL8;
		mAttachment = GL_DEPTH_STENCIL_ATTACHMENT;		
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
	}
	else
	{
		mInternalFormat = GL_DEPTH_COMPONENT32F;
		mAttachment = GL_DEPTH_ATTACHMENT;
		glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}
	//防止人工痕迹
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

GLHardwareDepthBuffer::~GLHardwareDepthBuffer()
{
	glDeleteRenderbuffers(1, &mDepthBufferID);
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
			glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);
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

void GLHardwareDepthBuffer::SetBufferMipmapImp(int InMipmapLevelNum)
{
	size_t maxMipmaps = GLPixelUtil::getMaxMipmaps(mWidth, mHeight, 1);
	if (MipmapLevelNum > maxMipmaps)
	{
		MipmapLevelNum = maxMipmaps;
	}
	if (MipmapLevelNum == 0)
	{
		MipmapLevelNum = 1;
	}
	GLint CurrentBindTextureID;
	GLenum CurrentTextureType =  TEX_TYPE_2D ;
	glGetIntegerv(CurrentTextureType, &CurrentBindTextureID);
	glBindTexture(GL_TEXTURE_2D, mDepthBufferID);
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0));
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, MipmapLevelNum - 1));//从0开始计数//开启这一项后会导致绑定到framebuffer上出现问题
	for (int mip = 0; mip < MipmapLevelNum; mip++)
	{
		int MipWidth = mWidth * std::pow(0.5, mip);
		int MipHeight = mHeight  * std::pow(0.5, mip);
		CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, mip, mInternalFormat, MipWidth, MipHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL));
	}
	
	//glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, CurrentBindTextureID);
	return;
}
