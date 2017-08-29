#ifndef GLRENDERTEXTURE_H_
#define GLRENDERTEXTURE_H_
#include "../BWEngineCore/BWRenderTexture.h"
#include "../BWEngineCore/BWSingleton.h"
#include "GLRenderBuffer.h"
// 描述了一个可以被渲染到的二维buffer  例如绑定到framebuffer中的深度缓存 颜色缓存等
class GLSurfaceDesc
{
public:
	GLRenderBuffer *buffer;
	size_t zoffset;
	unsigned int samples;
	GLSurfaceDesc() : buffer(NULL), zoffset(0), samples(0)
	{

	}
protected:
private:
};

//class GLRenderTexture : public BWRenderTexture
//{
//public:
//	GLRenderTexture(const std::string &name, const GLSurfaceDesc &target, bool writeGamma, unsigned int fsaa);
//	virtual ~GLRenderTexture();
//	bool requiresTextureFlipping() const { return true; }
//};
//
//class GLRTTManager : public BWSingleton<GLRTTManager>
//{
//public:
//	virtual BWRenderTexture*  createRenderTexture(const std::string &name, const GLSurfaceDesc &target,
//		bool wirteGamma, unsigned int fsaa) = 0;
//	virtual bool checkFormat(PixelFormat format) = 0;
//	virtual void bind(BWRenderTarget *target) = 0;
//	virtual void unbind(BWRenderTarget *target) = 0;
//	virtual BWMultiRenderTarget* createMultiRenderTarget(const std::string &name);
//	virtual PixelFormat getSupporteAlternative(PixelFormat format);
//};
#endif