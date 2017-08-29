#include "GLRenderBuffer.h"
#include "GLPixelUtil.h"
#include "../BWEngineCore/BWRenderTarget.h"
#include "../BWEngineCore/BWLog.h"
#include "GLRenderTarget.h"
#include "GLPreDefine.h"
GLHardwarePixelBuffer::GLHardwarePixelBuffer(const std::string &name ,unsigned int width , unsigned int height, PixelFormat format ,
	BWRenderTarget *creator)
	:BWHardwarePixelBuffer(width, height, 0, format, HBU_STATIC , false , false)
{
	glGenRenderbuffers(1, &mRenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferID);
	mInternalFormat = GLPixelUtil::getClosestGLInternalFormat(format);

    // 这里可以设置采样等信息
	//if (contentType == CT_Depth)
	//{
	//	mInternalFormat = GL_DEPTH_COMPONENT;
	//}
	//else
	//{
	//	mInternalFormat = GLPixelUtil::getClosestGLInternalFormat(format);
	//}
	mInternalFormat = GL_RGBA;
	CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER,mInternalFormat, width, height));

	mName = name;
	//mBufferType = type;
	mWidth = width;
	mHeight = height;
}

GLHardwarePixelBuffer::~GLHardwarePixelBuffer()
{
	glDeleteRenderbuffers(1, &mRenderbufferID);
}

bool GLHardwarePixelBuffer::attachToRenderTarget(BWRenderTarget *renderTarget)
{
	if (mRenderTarget)
	{
		mRenderTarget->removePixelRenderBuffer(mName);
	}
	mRenderTarget = renderTarget;
	GLRenderTarget *frameBuffer = dynamic_cast<GLRenderTarget*>(renderTarget);
	assert(frameBuffer);

	mAttachment = frameBuffer->getColorAttachment();
	//switch (mContentType)
	//{
	//case CT_Color:
	//	mAttachment = frameBuffer->getColorAttachment();
	//	break;
	//case CT_Depth:
	//	mAttachment = GL_DEPTH_ATTACHMENT;
	//	break;
	//case CT_Stencil:
	//	mAttachment = GL_STENCIL_ATTACHMENT;
	//	break;
	//case  CT_Stencil_Depth:
	//	mAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
	//	break;
	//default:
	//	break;
	//}
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->getID()));
	CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, mAttachment, GL_RENDERBUFFER, mRenderbufferID));
	GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (statues == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)//某个绑定到attachment上的元素无效
	{
		Log::GetInstance()->logMessage("GLHardwarePixelBuffer::attachToRenderTarget() : Error");
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void GLHardwarePixelBuffer::removeFromRenderTarget()
{
	if (mRenderTarget)
	{
		mRenderTarget->removePixelRenderBuffer(mName);
		mRenderTarget = NULL;
	}
}

void GLHardwarePixelBuffer::RemoveFromRenderTarget()
{
	if (mRenderTarget)
	{
		mRenderTarget->removePixelRenderBuffer(mName);
	}
}

GLenum GLHardwarePixelBuffer::getAttachment() const 
{
	return mAttachment;
}

void GLHardwarePixelBuffer::unLockImp()
{

 }
BWPixelBox GLHardwarePixelBuffer::lockImp(const BWBox lockBox, LockOptions option)
{
	BWPixelBox tmp;
	return tmp;
 }
void GLHardwarePixelBuffer::blitFromMemory(const BWPixelBox &src, const BWBox &desBox)
{

 }
void GLHardwarePixelBuffer::blitToMemory(const BWBox &srcBox, const BWPixelBox &des)
{

 }