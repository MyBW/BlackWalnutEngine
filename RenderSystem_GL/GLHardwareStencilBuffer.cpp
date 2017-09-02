#include "GLHardwareStencilBuffer.h"
#include "GLRenderTarget.h"
#include "../BWEngineCore/BWRenderTarget.h"
#include"GLHardwareDepthBuffer.h"
#include "../BWEngineCore/BWLog.h"
#include"GLPreDefine.h"
GLHardwareStencilBuffer::GLHardwareStencilBuffer(const std::string &name , size_t width, size_t height, size_t depth,
	BWRenderTarget* creator ,BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareStencilBuffer(width, height, depth, usage, useSystemMemory, useShadowBuffer)
{
	mName = name;
	mRenderTarget = creator;
	if (!mRenderTarget->hasDepthRenderBuffer())
	{
		mRenderTarget->createDepthBuffer(mName);
	}
	GLHardwareDepthBuffer *DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>( mRenderTarget->getDepthRenderBuffer(mName).Get());
	mStencilBufferID = DepthBuffer->GetGLID();
	CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, mStencilBufferID));
	mInternalFormat = GL_DEPTH24_STENCIL8; // 这里的格式需要更换
	CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, mInternalFormat, width, height));
	mAttachment = GL_STENCIL_ATTACHMENT;


	//mName = name;
	//mRenderTarget = creator;

	//glGenRenderbuffers(1, &mStencilBufferID);
	//glBindRenderbuffer(GL_RENDERBUFFER, mStencilBufferID);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, width, height);
	//
	//mInternalFormat = GL_STENCIL_INDEX8;
	//mAttachment = GL_STENCIL_ATTACHMENT;

}

GLHardwareStencilBuffer::~GLHardwareStencilBuffer()
{
	glDeleteRenderbuffers(1, &mStencilBufferID);
}

bool GLHardwareStencilBuffer::attachToRenderTarget(BWRenderTarget* renderTarget)
{
	if (mRenderTarget)
	{
		//mRenderTarget->removeRenderBuffer(mName);
	}
	mRenderTarget = renderTarget;
	GLRenderTarget *frameBuffer = dynamic_cast<GLRenderTarget*>(renderTarget);
	assert(frameBuffer);
	
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getID()));
	CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mStencilBufferID));
	CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, mAttachment, GL_RENDERBUFFER, mStencilBufferID));

	//GLRenderTarget *frameBuffer = dynamic_cast<GLRenderTarget*>(renderTarget);
	//CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getID()));
	//glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_STENCIL_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, mStencilBufferID);


	GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (statues == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)//某个绑定到attachment上的元素无效
	{
		Log::GetInstance()->logMessage("GLRenderBuffer::attachToRenderTarget() : Error");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void GLHardwareStencilBuffer::RemoveFromRenderTarget()
{
	if (mRenderTarget)
	{
		mRenderTarget->removeStencilRenderBuffer(mName);
		mRenderTarget = NULL;
	}
}

void* GLHardwareStencilBuffer::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	return  NULL;
}
void  GLHardwareStencilBuffer::unLockImp()
{

}