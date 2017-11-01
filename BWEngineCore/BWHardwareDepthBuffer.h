#ifndef HARDWAREDEPTHBUFFER_H_
#define HARDWAREDEPTHBUFFER_H_
#include "BWSmartPointer.h"
#include "BWHardwareBuffer.h"
#include "BWPrimitive.h"
#include "BWCommon.h"
class BWHardwareDepthBuffer;
class BWRenderTarget;

typedef SmartPointer<BWHardwareDepthBuffer> BWHardwareDepthBufferPtr;
class BWHardwareDepthBuffer: public BWHardwareBuffer
{
public:
	BWHardwareDepthBuffer(const std::string &Name ,size_t width, size_t height, size_t depth,
		BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
	virtual bool attachToRenderTarget(BWRenderTarget* renderTarget) = 0;
	FORCEINLINE bool GetIsWithStencil() { return IsWithStencil; }
	FORCEINLINE void SetRenderTarget(BWRenderTarget* RenderTarget) { mRenderTarget = RenderTarget; }
	virtual void SetIsWithStencil(bool IsWithStencil) { this->IsWithStencil = IsWithStencil; }
	virtual void RemoveFromRenderTarget() { };
	// 将DepthBuffer绑定到FBO后 如果频繁的改变DepthBuffer的大小 会导致显卡驱动崩溃 目前不知道什么原因
	void SetHardwareBufferSize(int Width, int Height);
	void SetHardwareBufferMipmap(int InMipmapLevelNum);
protected:
	virtual void SetBufferSizeImp(int Width , int Heigh) {  };
	virtual void SetBufferMipmapImp(int InMipmapLevelNum) { };
	BWRenderTarget *mRenderTarget;
	bool IsWithStencil;
	int mWidth, mHeight, mDepth;
	int MipmapLevelNum{ 1 };
};



#endif

