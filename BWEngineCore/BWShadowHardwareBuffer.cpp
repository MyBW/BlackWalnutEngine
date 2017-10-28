#include "BWShadowHardwareBuffer.h"

BWVertexShadowHardwareBuffer::BWVertexShadowHardwareBuffer(BWHardwareBufferManagerBase *manager,const std::string &Name, size_t vertexSize, size_t vertexNum, Usage usage, bool isSystemMemory)
:BWHardwareVertexBuffer(manager, Name, vertexSize, vertexNum, usage, false, false)
{
	mData = new unsigned char[mSizeInBytes];
}

BWVertexShadowHardwareBuffer::~BWVertexShadowHardwareBuffer()
{
	if (mData)
	{
		delete[] mData;
	}
	mData = nullptr;
}

void *BWVertexShadowHardwareBuffer::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	return  mData + offset;
}
void  BWVertexShadowHardwareBuffer::unLockImp()
{

}
void *BWVertexShadowHardwareBuffer::lock(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	mIsLocked = true;
	return mData + offset;
}
void BWVertexShadowHardwareBuffer::unlock()
{
	mIsLocked = false;
}


BWIndexShadowHardwareBuffer::BWIndexShadowHardwareBuffer(BWHardwareBufferManagerBase * manager, const std::string& Name, IndexType indexType, size_t indexNum, Usage usage, bool isUseSystem)
:BWHardwareIndexBuffer(manager, Name, indexType, indexNum, usage, isUseSystem, false)
{
	mData = new unsigned char[mSizeInBytes];
}
void *BWIndexShadowHardwareBuffer::lock(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	mIsLocked = true;
	return mData + offset;
}
void BWIndexShadowHardwareBuffer::unlock()
{
	mIsLocked = false;
}
void* BWIndexShadowHardwareBuffer::lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option)
{
	return mData + offset;
}
void BWIndexShadowHardwareBuffer::unLockImp()
{
}