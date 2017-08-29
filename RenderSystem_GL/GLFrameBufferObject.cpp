//#include "GLFrameBufferObject.h"
//
//GLFrameBufferObject::GLFrameBufferObject(GLFBOManager *manager, unsigned int fsaa) :
//mCreator(manager),
//mNumSamples(fsaa)
//{
//	glGenFramebuffers(1, &mFBID);
//    if (GLEW_EXT_framebuffer_blit && GLEW_EXT_framebuffer_multisample)
//    {
//		glBindFramebuffer(GL_FRAMEBUFFER, mFBID);
//		GLint maxSample;
//		glGetIntegerv(GL_MAX_SAMPLES, &maxSample);
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		mNumSamples = mNumSamples > maxSample ? mNumSamples : maxSample;
//    }
//	else
//	{
//		mNumSamples = 0;
//	}
//	if (mNumSamples)
//	{
//		glGenFramebuffers(1, &mMuiltsampleFB);
//	}
//	else
//	{
//		mMuiltsampleFB = 0;
//	}
//
//	mDepthBuffer.buffer = 0;
//	mStencilBuffer.buffer = 0;
//	for (size_t i = 0; i < OGRE_MAX_MULTIPLE_RENDER_TARGETS; i++)
//	{
//		mColourBuffer[i].buffer = 0;
//	}
//}
//void GLFrameBufferObject::bind()
//{
//	if (mMuiltsampleFB)
//	{
//		glBindFramebuffer(GL_FRAMEBUFFER, mMuiltsampleFB);
//	}
//	else
//	{
//		glBindFramebuffer(GL_FRAMEBUFFER, mFBID);
//	}
//}
//void GLFrameBufferObject::bindSurface(size_t attachment, const GLSurfaceDesc &target)
//{
//	assert(attachment < OGRE_MAX_MULTIPLE_RENDER_TARGETS);
//	mColourBuffer[attachment] = target;
//	if (mColourBuffer[0].buffer)
//	{
//		initialise();
//	}
//}
//void GLFrameBufferObject::initialise()
//{
//	mCreator->releaseRenderBuffer(mDepthBuffer);
//	mCreator->releaseRenderBuffer(mStencilBuffer);
//	mCreator->releaseRenderBuffer(mMuiltsampleColourBuffer);
//	if (!mColourBuffer[0].buffer)
//	{
//		assert(0); // 至少有一个颜色buffer
//	}
//	size_t width = mColourBuffer[0].buffer->getWidth();
//	size_t height = mColourBuffer[0].buffer->getHeight();
//	GLuint format = mColourBuffer[0].buffer->getGLFormat();
//	PixelFormat ogreformat = mColourBuffer[0].buffer->getFormat();
//
//	glBindFramebuffer(GL_FRAMEBUFFER, mFBID);
//	for (size_t i = 0; i < OGRE_MAX_MULTIPLE_RENDER_TARGETS; i++)
//	{
//		if (mColourBuffer[i].buffer)
//		{
//			if (mColourBuffer[i].buffer->getWidth() != width  || mColourBuffer[i].buffer->getHeight() != height)
//			{
//				assert(0);
//			}
//			if (mColourBuffer[i].buffer->getGLFormat() != format)
//			{
//				assert(0);
//			}
//			mColourBuffer[i].buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0 + i, mColourBuffer[i].zoffset);
//		}
//		else
//		{
//			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, 0);
//		}
//	}
//	if (mMuiltsampleFB)
//	{
//		glBindFramebuffer(GL_FRAMEBUFFER, mMuiltsampleFB);
//		mMuiltsampleColourBuffer = mCreator->requestRenderBuffer(format, width, height, mNumSamples);
//		mMuiltsampleColourBuffer.buffer->bindToFramebuffer(GL_COLOR_ATTACHMENT0, mMuiltsampleColourBuffer.zoffset);
//	}
//	GLenum depthFormat, stencilFormat;
//	mCreator->getBestDepthStencil(ogreformat, &depthFormat, &stencilFormat);
//	mDepthBuffer = mCreator->requestRenderBuffer(depthFormat, width, height, mNumSamples);
//	if (depthFormat == GL_DEPTH24_STENCIL8)
//	{
//		mCreator->requestRenderBuffer(mDepthBuffer);
//		mStencilBuffer = mDepthBuffer;
//	}
//	else
//	{
//		mStencilBuffer = mCreator->requestRenderBuffer(stencilFormat, width, height, mNumSamples);
//	}
//	if (mDepthBuffer.buffer)
//	{
//		mDepthBuffer.buffer->bindToFramebuffer(GL_DEPTH_ATTACHMENT, mDepthBuffer.zoffset);
//	}
//	else
//	{
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
//	}
//	if (mStencilBuffer.buffer)
//	{
//		mStencilBuffer.buffer->bindToFramebuffer(GL_STENCIL_ATTACHMENT, mStencilBuffer.zoffset);
//	}
//	else
//	{
//		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
//	}
//
//	GLenum bufs[OGRE_MAX_MULTIPLE_RENDER_TARGETS];
//	GLsizei n = 0;
//	for (size_t x = 0; x < OGRE_MAX_MULTIPLE_RENDER_TARGETS; x++)
//	{
//		if (mColourBuffer[x].buffer)
//		{
//			bufs[x] = GL_COLOR_ATTACHMENT0 + x;
//			n = x + 1;
//		}
//		else
//		{
//			bufs[x] = GL_NONE;
//		}
//	}
//	if (glDrawBuffers)
//	{
//		glDrawBuffers(n, bufs);
//	}
//	else
//	{
//		assert(0);
//	}
//	if (mMuiltsampleFB)
//	{
//		glReadBuffer(bufs[0]);
//	}
//	else
//	{
//		glReadBuffer(GL_NONE);
//	}
//	GLuint status;
//	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//	switch (status)
//	{
//	case GL_FRAMEBUFFER_COMPLETE:
//		break;
//	case GL_FRAMEBUFFER_UNSUPPORTED:
//		assert(0);
//		break;
//	default:
//		assert(0);
//		break;
//	}
//
//}