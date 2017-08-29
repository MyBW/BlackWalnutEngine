#ifndef PIXELUTIL_H_
#define PIXELUTIL_H_
#include "GL/include/glew.h"
#include "../BWEngineCore/BWPixlFormat.h"
/*
  类名：GLPixelUtil  
  作用：将引擎中定义的Pixe相关的类型转换为GL可以接收到额类型
*/
class GLPixelUtil
{
public:
	GLPixelUtil(){}
	~GLPixelUtil(){}
	static size_t optionalPO2(size_t src);
	static size_t getMaxMipmaps(size_t width, size_t height, size_t Depth);
	static GLenum  getClosestGLInternalFormat( PixelFormat format,bool hwGamma = false);
	static GLenum  getGLInternalFormat(PixelFormat format, bool hwGamma = false);
	static GLenum getGLOriginFormat(PixelFormat format);
	static GLenum getGLOriginDataType(PixelFormat format);
	static PixelFormat getClosestOGREFormat(GLenum fomat);
private:

};


#endif