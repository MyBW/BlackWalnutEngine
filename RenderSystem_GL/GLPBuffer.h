#ifndef GLPBUFFER_H_
#define GLPBUFFER_H_
#include "GLContext.h"
// 一个离屏渲染的上下文  该格式为RGBA 
class GLPBuffer
{
public:
	GLPBuffer(PixelComponentType format, size_t width, size_t height);
	virtual ~GLPBuffer();
	virtual GLContext* getContext() = 0;
	PixelComponentType getFormat() { return mFormat; }
	size_t getWidth(){ return mWidth; }
	size_t getHeight(){ return mHeight; }
	static PixelComponentType getPixelComponentType(PixelFormat fmt);
protected:
	PixelComponentType mFormat;
	size_t mHeight;
	size_t mWidth;
private:
};
#endif