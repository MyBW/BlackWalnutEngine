//#include "GLFBOManager.h"
//static const size_t depthBits[] = { 0, 16, 24, 32, 23 };
//static const size_t stencilBits[] = { 0, 1, 4, 8, 16 };
//static const GLenum depthFormats[] = {
//	GL_NONE,
//	GL_DEPTH_COMPONENT16,
//	GL_DEPTH_COMPONENT24,
//	GL_DEPTH_COMPONENT32,
//	GL_DEPTH24_STENCIL8
//};
//static const GLenum stencilFormats[] = {
//	GL_NONE,
//	GL_STENCIL_INDEX1,
//	GL_STENCIL_INDEX4,
//	GL_STENCIL_INDEX8,
//	GL_STENCIL_INDEX16
//};
//GLSurfaceDesc GLFBOManager::requestRenderBuffer(GLenum format, size_t width, size_t height, unsigned int fsaa)
//{
//	/*GLSurfaceDesc face;
//	face.buffer = 0;
//	if (format != GL_NONE)
//	{
//		RBFFormat key(format, width, height, fsaa);
//		RenderBufferMap::iterator itor = mRenderBufferMap.find(key);
//		if (itor != mRenderBufferMap.end())
//		{
//			face.buffer = itor->second.buffer;
//			face.zoffset = 0;
//			face.samples = fsaa;
//			++(itor->second.refCount);
//		}
//		else
//		{
//			GLRenderBuffer *rb = new GLRenderBuffer(format, width, height, fsaa);
//			mRenderBufferMap[key] = RBRef(rb);
//			face.buffer = rb;
//			face.zoffset = 0;
//			face.samples = fsaa;
//		}
//	}
//	return face;*/
//	GLSurfaceDesc face;
//	return face;
//}
//void GLFBOManager::requestRenderBuffer(const GLSurfaceDesc &buffer)
//{
//	/*if (buffer.buffer)
//	{
//		return;
//	}
//	RBFFormat key(buffer.buffer->getGLFormat(), buffer.buffer->getWidth(), buffer.buffer->getHeight(), buffer.samples);
//	RenderBufferMap::iterator itor = mRenderBufferMap.find(key);
//	assert(itor != mRenderBufferMap.end());
//	if (itor != mRenderBufferMap.end())
//	{
//		assert(itor->second.buffer == buffer.buffer);
//		++(itor->second.refCount);
//	}*/
//}
//void GLFBOManager::getBestDepthStencil(GLenum internalFormat, GLenum *depthFormat, GLenum *stencilFormat)
//{
//	const FormatProperties &props = mProps[internalFormat];
//	size_t bestnode = 0;
//	int bestscore = -1;
//	for (size_t mode = 0; mode < props.modes.size(); mode++)
//	{
//		int desirability = 0;
//		if (props.modes[mode].stencil)
//		{
//			desirability += 1000;
//		}
//		if (props.modes[mode].depth)
//		{
//			desirability += 2000;
//		}
//		if (depthBits[props.modes[mode].depth] == 24)
//		{
//			desirability += 500;
//		}
//		if (depthFormats[props.modes[mode].depth] == GL_DEPTH24_STENCIL8)
//		{
//			desirability += 5000;
//		}
//		desirability += stencilBits[props.modes[mode].stencil] + depthBits[props.modes[mode].depth];
//		if (desirability > bestscore)
//		{
//			bestscore = desirability;
//			bestnode = mode;
//		}
//	}
//	*depthFormat = depthFormats[props.modes[bestnode].depth];
//	*stencilFormat = stencilFormats[props.modes[bestnode].stencil];
//}
//
//void GLFBOManager::releaseRenderBuffer(GLSurfaceDesc &buffer)
//{
//	assert(0);
//}