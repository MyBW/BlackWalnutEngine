#ifndef HARDWAREPIXELBUFFER_H_
#define HARDWAREPIXELBUFFER_H_
#include "BWSmartPointer.h"
#include "BWHardwareBuffer.h"
#include "BWPixlFormat.h"
#include "BWPrimitive.h"

class BWHardwarePixelBuffer;
class BWRenderTarget;
typedef SmartPointer<BWHardwarePixelBuffer> BWHardwarePixelBufferPtr;

enum RenderBufferType
{
	TY_RenderBuffer,
	TY_TextureBuffer
};

class BWHardwarePixelBuffer : public BWHardwareBuffer
{

protected:
	size_t mWidth, mHeight, mDepth;
	size_t mRowPitch, mSlicePitch;
	PixelFormat mFormat;
	BWPixelBox mCurrentLock;
	BWBox mLockedBox;
	BWRenderTarget* mRenderTarget;
	RenderBufferType  mBufferType;

	virtual BWPixelBox lockImp(const BWBox lockBox, LockOptions option) = 0;
	virtual void * lockImp(size_t  offset, size_t lenth, LockOptions option);
	virtual void _clearSliceRTT(size_t zoffset) { assert(0); }
public:
	BWHardwarePixelBuffer(size_t width , size_t height , size_t depth , PixelFormat format 
		, BWHardwareBuffer::Usage usage , bool useSystemMemory , bool useShadowBuffer);
	virtual ~BWHardwarePixelBuffer();
	using BWHardwareBuffer::lock;
	virtual const BWPixelBox& lock(const BWBox &lockBox, LockOptions option);
	virtual void *lock(size_t offset, size_t length, LockOptions option);
	const BWPixelBox & getCurrentLock();
	virtual void readData(size_t offset, size_t length, void *pDest);
	virtual void writeData(size_t offset, size_t length, const void *pSrc, bool discardWholeBuffer = false);
	virtual void blit(const BWHardwarePixelBufferPtr &src, const BWBox &srcBox, const BWBox &destBox);
	void blit(const BWHardwarePixelBufferPtr &src);
	virtual void blitFromMemory(const BWPixelBox &src, const BWBox &desBox) = 0;
	void blitFromMemory(const BWPixelBox &src)
	{
		blitFromMemory(src, BWBox(0, 0, 0, mWidth, mHeight, mDepth));
	}
	virtual void blitToMemory(const BWBox &srcBox, const BWPixelBox &des) = 0;
	void blitToMemory(const BWPixelBox &dest)
	{
		blitToMemory(BWBox(0, 0, 0, mWidth, mHeight, mDepth), dest);
	}
	virtual bool attachToRenderTarget(BWRenderTarget *renderTarget) = 0;
	bool isAttachToRenderTarget() const { return mRenderTarget != NULL; }
	virtual void RemoveFromRenderTarget() { };
	size_t getWidth(){ return mWidth; }
	size_t getHeight(){ return mHeight; }
	size_t getDepth(){ return mDepth; }
	PixelFormat getFormat(){ return mFormat; }
};


#endif