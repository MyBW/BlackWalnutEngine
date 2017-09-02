//#include "GLTextureRenderBuffer.h"
//#include "../BWEngineCore/BWCommon.h"
//#include "GLTextureManager.h"
//#include "GLRenderTarget.h"
//#include "GLPixelUtil.h"
//#include "../BWEngineCore/BWLog.h"
//#include "GLPreDefine.h"
///*GLTextureRenderBuffer*/::GLTextureRenderBuffer(const std::string &name , unsigned width, unsigned height, PixelFormat format ,
//	RenderBufferType type , RenderBufferContentType contentType , BWRenderTarget *creator)
//:BWRenderBuffer(width , height , format)
//{
//	// ����ʹ��createorretrieve �ȽϺ���  ����һ��texture ����texture���� Ȼ����loadtexture
//	mTexture = GLTextureManager::GetInstance()->load(name, DEFAULT_RESOURCE_GROUP , TU_RENDERTARGET , TEX_TYPE_2D , -1 , 1.0 , false , format);
//	
//	RenderBufferType mBufferType;
//	mName = name;
//	mContentType = contentType;
//	mBufferType = type;
//	mWidth = width;
//	mHeight = height;
//	mTexture->setHeight(height);
//	mTexture->setWidth(width);
//	// ��Ϊ�����ദ���ϻ�����һЩ���� ���� �����ض���һ��
//	// ������ʱֻ֧��2D����  ����Ҫע�� ���texture�󶨵�������� ʲô�죿��
//	if (contentType == CT_Depth)
//	{
//		mInternalFormat = GL_DEPTH_COMPONENT;
//	}
//	else
//	{
//		mInternalFormat = GLPixelUtil::getClosestGLInternalFormat(format);
//	}
//	GLTexturePtr glTexture = mTexture;
//	CHECK_GL_ERROR(glBindTexture(glTexture->GetGLTextureTarget(), glTexture->GetID()));
//
//	CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0,
//		mInternalFormat, 
//		glTexture->getWidth(), 
//		glTexture->getHeight(),
//		0, GL_RGBA, GL_UNSIGNED_BYTE, 0)
//		);
//
//	attachToRenderTarget(creator);
//}
//
//void GLTextureRenderBuffer::attachToRenderTarget(BWRenderTarget *renderTarget)
//{
//	GLTexturePtr glTexture = mTexture;
//	mRenderTarget = renderTarget;
//	GLRenderTarget *glRenderTarget = dynamic_cast<GLRenderTarget*> (renderTarget);
//	switch (mContentType)
//	{
//	case CT_Color:
//		mAttachment = glRenderTarget->getColorAttachment();
//		break;
//	case CT_Depth:
//		mAttachment = GL_DEPTH_ATTACHMENT;
//		break;
//	case CT_Stencil:
//		mAttachment = GL_STENCIL_ATTACHMENT;
//		break;
//	case  CT_Stencil_Depth:
//		mAttachment = GL_DEPTH_STENCIL_ATTACHMENT;
//		break;
//	default:
//		break;
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, glRenderTarget->getID());
//	glFramebufferTexture2D(GL_FRAMEBUFFER, mAttachment, GL_TEXTURE_2D, glTexture->GetID(), 0);
//	GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	if (statues == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)//ĳ���󶨵�attachment�ϵ�Ԫ����Ч
//	{
//		Log::GetInstance()->logMessage("GLRenderBuffer::attachToRenderTarget() : Error");
//	}
//}
//void GLTextureRenderBuffer::removeFromRenderTarget()
//{
//	if (mRenderTarget)
//	{
//		//mRenderTarget->removeRenderBuffer(mName);
//	}
//}
//GLenum GLTextureRenderBuffer::getAttachment() const 
//{
//	return mAttachment;
//}