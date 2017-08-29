#ifndef HARDWARESTENCILBUFFER_H_
#define HARDWARESTENCILBUFFER_H_
#include "BWSmartPointer.h"
#include "BWHardwareBuffer.h"
#include "BWPrimitive.h"
class BWHardwareStencilBuffer;
class BWRenderTarget;
typedef SmartPointer<BWHardwareStencilBuffer> BWHardwareStencilBufferPtr;

class BWHardwareStencilBuffer: public BWHardwareBuffer
{
public:
	BWHardwareStencilBuffer(size_t width, size_t height, size_t depth,
		BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
	virtual bool attachToRenderTarget(BWRenderTarget*) = 0;
	virtual void RemoveFromRenderTarget() { };
protected:
	BWRenderTarget* mRenderTarget;
private:
	size_t mWidth, mHeight, mDepth;
	
};


#endif