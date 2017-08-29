#include "BWHardwareBuffer.h"
#include <assert.h>
#include <memory>
BWHardwareBuffer::BWHardwareBuffer(Usage usage, bool systemMemory, bool isUseShadowBuffer)
:mUsage(usage)
, mUseShadowBuffer(isUseShadowBuffer)
, mSizeInBytes(0)
, mIsLocked(false)
, mShadowHardwareBuffer(NULL)
, mShadowUpdated(false)
, mLockStart(0)
, mLockLenth(0)
{

}
void * BWHardwareBuffer::lock(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	assert(!isLock());
	void *ret;
	if (mUseShadowBuffer)
	{
		if (option != HBL_READ_ONLY)
		{
			mShadowUpdated = true;
		}
		ret = mShadowHardwareBuffer->lock(offset, length, option);
		//mIsLocked = true; //这是后来加的
	}
	else
	{
		ret = lockImp(offset, length, option);
		mIsLocked = true;// 为什么要锁住？  lockImp函数不是已经执行完了么   因为这个函数返回的是申请buffer（GPU 或者 系统）的首地址 还没向内存传输数据 所以要锁住
	}
	mLockStart = offset;
	mLockLenth = length;
	return ret;
}

void * BWHardwareBuffer::lock(BWHardwareBuffer::LockOptions option)
{
	return this->lock(0, mSizeInBytes, option);
}
void BWHardwareBuffer::unlock()
{
	//assert(!isLock());  这里的逻辑有问题
	if (mUseShadowBuffer && mShadowHardwareBuffer->isLock())
	{
		mShadowHardwareBuffer->unlock();
		updateFromShadowBuffer();
	}
	else
	{
		unLockImp();
		mIsLocked = false;
	}
	return;
}

void BWHardwareBuffer::updateFromShadowBuffer()
{
	if (mUseShadowBuffer && mShadowUpdated /*&& !mSuppresshardwareBufferUpdata*/)
	{
		const void * srcData = mShadowHardwareBuffer->lockImp(mLockStart, mLockLenth, HBL_READ_ONLY);
		LockOptions option;
		if (mLockStart == 0 && mLockLenth == mSizeInBytes)
		{
			option = HBL_DISCARD;
		}
		else
		{
			option = HBL_NORMAL;
		}
		void *destData = this->lockImp(mLockStart, mLockLenth, option);
		memcpy(destData, srcData, mLockLenth);
		this->unLockImp();
		mShadowHardwareBuffer->unLockImp();
		mShadowUpdated = false;
	}
}
bool BWHardwareBuffer::isLock()
{
	return mIsLocked;
}
const std::string& BWHardwareBuffer::getName() const
{
	return mName;
}