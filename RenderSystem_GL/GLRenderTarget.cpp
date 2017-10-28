
#include "GLRenderTarget.h"
#include "GLRenderBuffer.h"
#include "GLTextureRenderBuffer.h"
#include "../BWEngineCore/BWLog.h"
#include "GLHardwareDepthBuffer.h"
#include "GLHardwareStencilBuffer.h"
#include "GLPreDefine.h"
#include "GLTexture.h"
GLint GLRenderTarget::Max_Color_Attachment_Number = 16;
GLRenderTarget::GLRenderTarget()
:mFrameBufferID(0),
mColorAttachMask(0),
mColorAttachNum(0)
{
	glGenFramebuffers(1, &mFrameBufferID);
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &Max_Color_Attachment_Number);
}

GLRenderTarget::~GLRenderTarget()
{
	Destroty();
	glDeleteFramebuffers(1, &mFrameBufferID);
}

GLenum GLRenderTarget::getColorAttachment()
{
	for (int i = 0; i < Max_Color_Attachment_Number; i++)
	{
		if (!(mColorAttachMask & (1<<i)))
		{
			mColorAttachNum++;
			mColorAttachMask += (1 << i);
			return GL_COLOR_ATTACHMENT0 + i;
		}
	}
	Log::GetInstance()->logMessage("GLRenderTarget::getColorAttachment : Error");
	assert(0);
  return GL_COLOR_ATTACHMENT0;
}

void GLRenderTarget::CollectAttachment(GLenum ColorAttachment)
{
	GLuint Bit = 1;
	int OffsetNum = (ColorAttachment - GL_COLOR_ATTACHMENT0);
	Bit = Bit << OffsetNum;
	Bit = ~Bit;
	mColorAttachMask &= Bit;
}

//BWRenderBuffer* GLRenderTarget::_createRenderBufferImp(const std::string &name, RenderBufferType bufferType, RenderBufferContentType contentType, BWRenderTarget *creator)
//{
//	BWRenderBuffer *renderBuffer = NULL;
//	/*switch (bufferType)
//	{
//	case TY_RenderBuffer:
//		renderBuffer = new GLRenderBuffer(name , creator->getWidth() , creator->getHeight() , creator->suggestPixelFormat() , bufferType , contentType , creator);
//		break;
//	case TY_TextureBuffer:
//		renderBuffer = new GLTextureRenderBuffer(name ,creator->getWidth(), creator->getHeight(), creator->suggestPixelFormat(), bufferType , contentType , creator);
//		break;
//	default:
//		break;
//	}*/
//	
//	return renderBuffer;
//}
//void GLRenderTarget::_removeRenderTargetImp(BWRenderBuffer *renderBuffer)
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);
//	GLenum attachment;
//	if (renderBuffer->getType() == TY_RenderBuffer)
//	{
//		GLRenderBuffer *glRenderBuffer = dynamic_cast<GLRenderBuffer*>(renderBuffer);
//		attachment = glRenderBuffer->getAttachment();
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment ,GL_RENDERBUFFER , 0);
//	}
//	else
//	{
//		GLTextureRenderBuffer *textureRenderBuffer = dynamic_cast<GLTextureRenderBuffer*>(renderBuffer);
//		attachment = textureRenderBuffer->getAttachment();
//		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
//	}
//	if (renderBuffer->getContentType() == CT_Color)
//	{
//		GLuint index = attachment - GL_COLOR_ATTACHMENT0;
//		GLuint tmp = 1;
//		mColorAttachMask = mColorAttachMask &(~(tmp << index));
//		mColorAttachNum--;
//	}
//}
GLuint GLRenderTarget::getID() const	
{
	return mFrameBufferID;
}

void GLRenderTarget::getCustomAttribute(const std::string &name, void *data)
{

}
void GLRenderTarget::copyContentsToMemory(const BWPixelBox &dst, FrameBuffer buffer)
{

}
GLuint GLRenderTarget::getColorAttachNum()
{
	return mColorAttachNum;
}
bool GLRenderTarget::requiresTextureFlipping() const
{
	return false;
}
BWHardwarePixelBufferPtr GLRenderTarget::_createHardwarePixelBufferImp(const std::string &name, RenderBufferType bufferType, BWRenderTarget *creator)
{
	return new GLHardwarePixelBuffer(name , creator->getWidth() ,creator->getHeight(), creator->suggestPixelFormat() ,creator);
}
BWHardwareStencilBufferPtr GLRenderTarget::_createHardwareStencilBufferImp(const std::string &name, BWRenderTarget *creator)
{
	return new GLHardwareStencilBuffer(name, creator->getWidth(), creator->getHeight(), 0, creator, BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
}
BWHardwareDepthBufferPtr GLRenderTarget::_createHardwareDepthBufferImp(const std::string &name, BWRenderTarget *creator)
{
	return new GLHardwareDepthBuffer(name, creator->getWidth(), creator->getHeight(),0,BWHardwareBuffer::Usage::HBU_DYNAMIC , false , false);
}

bool GLRenderTarget::_removeHardwareDepthBufferImp(BWHardwareDepthBufferPtr depthBuffer)
{
	GLHardwareDepthBuffer* DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>(depthBuffer.Get());
	/*CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, getID()));
	GLHardwareDepthBuffer* DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>(depthBuffer.Get());
	glFramebufferTexture2D(GL_FRAMEBUFFER, DepthBuffer->GetAttachment(), GL_TEXTURE_2D, 0, 0);*/
	DepthBuffer->SetRenderTarget(NULL);
	return true;
}

bool GLRenderTarget::_removeHardwarePixelBufferImp(BWHardwarePixelBufferPtr pixelBuffer)
{
	return true;
}

bool GLRenderTarget::_removeHardwareStencilBufferImp(BWHardwareStencilBufferPtr stencilBuffer)
{
	return  true;
}
bool GLRenderTarget::_removeTextureBufferImp(BWTexturePtr texture)
{
	texture->RemoveFromRenderTarget();
	return true;
}
void GLRenderTarget::_activeColorRenderBuffer()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);
}
void GLRenderTarget::setDrawBuffers(const NameLocation &BufferNameLocation)
{
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID));

	int ColorBufferNum = BufferNameLocation.size();
	int ActiveColorBufferNum = 0;
	GLenum *ActiveBuffer = new GLenum[ColorBufferNum];
	NameLocation::const_iterator namelocation = BufferNameLocation.begin();
	while (namelocation != BufferNameLocation.end())
	{
		TextureBufferMap::iterator textureItor = mTextureBufferMap.find(namelocation->first);
		if (textureItor != mTextureBufferMap.end())
		{
			ActiveBuffer[ActiveColorBufferNum] = dynamic_cast<GLTexture*>(textureItor->second.Get())->getTextureBufferAttachment();
			ActiveColorBufferNum++;
		}
		PixelBufferMap::iterator pixelItor = mRenderBufferMap.find(namelocation->first);
		if(pixelItor != mRenderBufferMap.end())
		{
			ActiveBuffer[ActiveColorBufferNum] = dynamic_cast<GLHardwarePixelBuffer*>(pixelItor->second.Get())->getAttachment();
			ActiveColorBufferNum++;
		}
		namelocation++;
	}
	CHECK_GL_ERROR(glDrawBuffers(ActiveColorBufferNum, ActiveBuffer));
	//CHECK_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	delete[] ActiveBuffer;
}
void GLRenderTarget::clearRenderTarget()
{
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID));
	int ColorBufferNum = mTextureBufferMap.size() + mRenderBufferMap.size();
	int ActiveColorBufferNum = 0;
	GLenum *ActiveBuffer = new GLenum[ColorBufferNum];
	TextureBufferMap::iterator textureItor = mTextureBufferMap.begin();
	while (textureItor != mTextureBufferMap.end())
	{
		ActiveBuffer[ActiveColorBufferNum] = dynamic_cast<GLTexture*>(textureItor->second.Get())->getTextureBufferAttachment();
		textureItor++;
		ActiveColorBufferNum++;
	}
			
	PixelBufferMap::iterator pixelItor = mRenderBufferMap.begin();
	while (pixelItor != mRenderBufferMap.end())
	{
		ActiveBuffer[ActiveColorBufferNum] = dynamic_cast<GLHardwarePixelBuffer*>(pixelItor->second.Get())->getAttachment();
		pixelItor++;
		ActiveColorBufferNum++;
	}
	CHECK_GL_ERROR(glDrawBuffers(ActiveColorBufferNum, ActiveBuffer));
	glClearColor(0.0, 0.0, 0.0, 0.0);

	GLbitfield clearTag = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

	
	if (mStencilBufferMap.size())
		clearTag |= GL_STENCIL_BUFFER_BIT;
	Log::GetInstance()->logMessage(std::string("StencilBuffer All Always Clear!!!!!!!!!!!!!"));
	clearTag |= GL_STENCIL_BUFFER_BIT;


	CHECK_GL_ERROR(glClear(clearTag));
	delete[] ActiveBuffer;
}

void GLRenderTarget::CopyToScreenFromColorBuffer(const std::string& ColorBufferName)
{
	GLenum AttachEnum;
	if (getPixelRenderBuffer(ColorBufferName).Get())
	{
		GLHardwarePixelBuffer * PixelBuffer = dynamic_cast<GLHardwarePixelBuffer*>(getPixelRenderBuffer(ColorBufferName).Get());
		AttachEnum = PixelBuffer->getAttachment();
	}
	else if (getTextureBuffer(ColorBufferName).Get())
	{
		GLTexture* TextureBuffer = dynamic_cast<GLTexture*>(getTextureBuffer(ColorBufferName).Get());
		AttachEnum = TextureBuffer->getTextureBufferAttachment();
	}
	else
	{
		Log::GetInstance()->logMessage(std::string("GLRenderTarget::CopyToScreenFromColorBuffer"));
		assert(0);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, getID()));
	CHECK_GL_ERROR(glReadBuffer(AttachEnum));
	CHECK_GL_ERROR(glBlitFramebuffer(0, 0, getWidth(), getHeight(),
		0, 0, getWidth(), getHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR));
}

void GLRenderTarget::Destroty()
{
	BWRenderTarget::Destroty();
}

void GLRenderTarget::_setRenderTarget()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferID);
	_activeColorRenderBuffer();
}
void GLRenderTarget::_removeRenderTarget()
{
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GLRenderTarget::_endUpdate()
{
	 //将数据移动到屏幕上
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, 1));
    CHECK_GL_ERROR(glReadBuffer(GL_COLOR_ATTACHMENT0));
	CHECK_GL_ERROR(glBlitFramebuffer(0, 0, getWidth(), getHeight(),
		0, 0, getWidth()/2, getHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR));
	CHECK_GL_ERROR(glReadBuffer(GL_COLOR_ATTACHMENT1));
	CHECK_GL_ERROR(glBlitFramebuffer(0, 0, getWidth(), getHeight(),
		getWidth()/2, 0, getWidth(), getHeight(), GL_COLOR_BUFFER_BIT, GL_LINEAR));
    CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 1));
	GLenum list [] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, list);
	CHECK_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, 0));*/
}