#include "BWHardwarePixelBuffer.h"
#include "BWImage.h"
BWHardwarePixelBuffer::BWHardwarePixelBuffer(const std::string &Name, size_t width, size_t height, size_t depth, PixelFormat format,
	BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareBuffer(Name, usage, useSystemMemory, useShadowBuffer),
	mWidth(width),
	mHeight(height),
	mDepth(depth),
	mFormat(format)
{
	mRowPitch = mWidth;
	mSlicePitch = mWidth * mHeight;
	mSizeInBytes = mWidth * mHeight * PixelUtil::getNumElemBytes(mFormat);
	mRenderTarget = NULL;
}
BWHardwarePixelBuffer::~BWHardwarePixelBuffer()
{

}

void *BWHardwarePixelBuffer::lock(size_t offset, size_t lenth, LockOptions option)
{
	assert(!isLock());
	assert(offset == 0 && lenth == mSizeInBytes);// 这里要求使用整个BWPixelBox
	BWBox box(0, mWidth, 0, mHeight, 0, mDepth);
	const BWPixelBox &rv = lock(box, option);
	return rv.mData;
}
const BWPixelBox & BWHardwarePixelBuffer::lock(const BWBox &lockBox, LockOptions option)
{
	if (mUseShadowBuffer)
	{
		if (option != HBL_READ_ONLY)
		{
			mShadowUpdated = true;
		}
		mCurrentLock = static_cast<BWHardwarePixelBuffer*>(mShadowHardwareBuffer)->lock(lockBox, option);
	}
	else
	{
		mCurrentLock = lockImp(lockBox, option);
		mIsLocked = true;
	}
	return mCurrentLock;
}

const BWPixelBox& BWHardwarePixelBuffer::getCurrentLock()
{
	assert(isLock() && "Cannot get current lock: buffer not locked");

	return mCurrentLock;
}

//-----------------------------------------------------------------------------    
/// Internal implementation of lock()
void* BWHardwarePixelBuffer::lockImp(size_t offset, size_t length, LockOptions options)
{
	/*OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, "lockImpl(offset,length) is not valid for PixelBuffers and should never be called",
		"HardwarePixelBuffer::lockImpl");*/
	assert(0);
	return NULL;
}

//-----------------------------------------------------------------------------    

void BWHardwarePixelBuffer::blit(const BWHardwarePixelBufferPtr &src, const BWBox &srcBox, const BWBox &dstBox)
{
	if (isLock() || src->isLock())
	{
		/*OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR,
			"Source and destination buffer may not be locked!",
			"HardwarePixelBuffer::blit");*/
		assert(0);
	}
	if (src.Get() == this)
	{
		/*OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"Source must not be the same object",
			"HardwarePixelBuffer::blit");*/
		assert(0);
	}
	const BWPixelBox &srclock = src->lock(srcBox, HBL_READ_ONLY);

	LockOptions method = HBL_NORMAL;
	if (dstBox.left == 0 && dstBox.top == 0 && dstBox.front == 0 &&
		dstBox.right == mWidth && dstBox.bottom == mHeight &&
		dstBox.back == mDepth)
		// Entire buffer -- we can discard the previous contents
		method = HBL_DISCARD;

	const BWPixelBox &dstlock = lock(dstBox, method);
	if (dstlock.getWidth() != srclock.getWidth() ||
		dstlock.getHeight() != srclock.getHeight() ||
		dstlock.getDepth() != srclock.getDepth())
	{
		// Scaling desired
		Image::scale(srclock, dstlock);
	}
	else
	{
		// No scaling needed
		PixelUtil::bulkPixelConversion(srclock, dstlock);
	}

	unlock();
	src->unlock();
}
//-----------------------------------------------------------------------------       
void BWHardwarePixelBuffer::blit(const BWHardwarePixelBufferPtr &src)
{
	blit(src,
		BWBox(0, 0, 0, src->getWidth(), src->getHeight(), src->getDepth()),
		BWBox(0, 0, 0, mWidth, mHeight, mDepth)
		);
}


//-----------------------------------------------------------------------------    
void BWHardwarePixelBuffer::readData(size_t offset, size_t length, void* pDest)
{
	// TODO
	/*OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
		"Reading a byte range is not implemented. Use blitToMemory.",
		"HardwarePixelBuffer::readData");*/
	assert(0);
}
//-----------------------------------------------------------------------------    

void BWHardwarePixelBuffer::writeData(size_t offset, size_t length, const void* pSource,
	bool discardWholeBuffer)
{
	//// TODO
	//OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
	//	"Writing a byte range is not implemented. Use blitFromMemory.",
	//	"HardwarePixelBuffer::writeData");
	assert(0);
}
//-----------------------------------------------------------------------------    

//RenderTexture *HardwarePixelBuffer::getRenderTarget(size_t)
//{
//	OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED,
//		"Not yet implemented for this rendersystem.",
//		"HardwarePixelBuffer::getRenderTarget");
//}

//-----------------------------------------------------------------------------    

//BWHardwarePixelBufferPtr::BWHardwarePixelBufferPtr(BWHardwarePixelBuffer* buf)
//: SharedPtr<BWHardwarePixelBuffer>(buf)
//{
//
//}
////-----------------------------------------------------------------------------    
//
//void BWHardwarePixelBuffer::_clearSliceRTT(size_t zoffset)
//{
//}

