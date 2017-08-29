#ifndef HARDWAREDEPTHBUFFER_H_
#define HARDWAREDEPTHBUFFER_H_
#include "BWSmartPointer.h"
#include "BWHardwareBuffer.h"
#include "BWPrimitive.h"
class BWHardwareDepthBuffer;
class BWRenderTarget;

typedef SmartPointer<BWHardwareDepthBuffer> BWHardwareDepthBufferPtr;
class BWHardwareDepthBuffer: public BWHardwareBuffer
{
public:
	BWHardwareDepthBuffer(size_t width, size_t height, size_t depth,
		BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
	virtual bool attachToRenderTarget(BWRenderTarget*) = 0;
	virtual void SetIsWithStencil(bool IsWithStencil) { this->IsWithStencil = IsWithStencil; }
	void SetRenderTarget(BWRenderTarget* RenderTarget) { mRenderTarget = RenderTarget; }
	void SetHardwareBufferSize(int Width, int Height);
	virtual void RemoveFromRenderTarget() { };
protected:
	virtual void SetBufferSizeImp(int Width , int Heigh) {  };
	BWRenderTarget *mRenderTarget;
	bool IsWithStencil;
	int mWidth, mHeight, mDepth;	
};



#endif

