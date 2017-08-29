//#ifndef GLFBOMANAGER_H_
//#define GLFBOMANAGER_H_
//#include "../BWEngineCore/BWSingleton.h"
//#include "GLRenderTexture.h"
//#include "GLRenderBuffer.h"
//class GLFBOManager;
//
//class GLFBORenderTexture : public GLRenderTexture
//{
//public:
//protected:
//private:
//};
//class GLFBOManager : public GLRTTManager
//{
//public:
//	void releaseRenderBuffer(GLSurfaceDesc &buffer);
//	void bind(BWRenderTarget *renderTarget);
//	void unbind(BWRenderTarget *renderTarget) { }
//
//	GLSurfaceDesc requestRenderBuffer(GLenum format, size_t width, size_t height, unsigned int fsaa);
//	void requestRenderBuffer(const GLSurfaceDesc &buffer);
//	void getBestDepthStencil(GLenum internalFormat, GLenum *depthFormat, GLenum *stencilFormat);
//protected:
//	struct RBFFormat
//	{
//		RBFFormat(GLenum f, size_t w, size_t h, unsigned int s) :format(f), width(w), height(h), sample(s)
//		{
//		}
//		GLenum format;
//		size_t width;
//		size_t height;
//		unsigned int sample;
//		bool operator <(const RBFFormat &other) const
//		{
//			if (format < other.format)
//			{
//				return true;
//			}
//			else if (format == other.format)
//			{
//				 if (width < other.width)
//				 {
//					 return true;
//				 }
//				 else if (width == other.width)
//				 {
//					 if (height < other.height)
//					 {
//						 return true;
//					 }else if (height == other.height)
//					 {
//						 if (sample < other.sample)
//						 {
//							 return true;
//						 }
//					 }
//				 }
//			}
//			return false;
//		}
//	};
//	struct RBRef
//	{
//		RBRef() {}
//		RBRef(GLRenderBuffer *inBuffer) :buffer(inBuffer), refCount(1)
//		{
//
//		}
//		GLRenderBuffer *buffer;
//		size_t refCount;
//	};
//	typedef std::map<RBFFormat, RBRef> RenderBufferMap;
//	struct FormatProperties
//	{
//		bool valid;
//		struct Mode
//		{
//			size_t depth;
//			size_t stencil;
//		};
//		std::vector<Mode> modes;
//	};
//	FormatProperties mProps[PF_COUNT];
//	RenderBufferMap mRenderBufferMap;
//private:
//};
//
//#endif