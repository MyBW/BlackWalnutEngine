#include "BWHardwareIndexBuffer.h"
#include "BWShadowHardwareBuffer.h"
BWHardwareIndexBuffer::BWHardwareIndexBuffer(BWHardwareBufferManagerBase *manager, IndexType indxType, size_t numberIndex, Usage usage, bool isUseSystemMemory, bool isUseShadowBuffer)
:BWHardwareBuffer(usage, false, isUseShadowBuffer), mIndexType(indxType), mIndexNum(numberIndex)
{
	switch (mIndexType)
	{
	case BWHardwareIndexBuffer::IT_16BIT:
		mIndexSize = sizeof(unsigned short);
		break;
	case BWHardwareIndexBuffer::IT_32BIT:
		mIndexSize = sizeof(unsigned int);
		break;
	default:
		break;
	}
	
	mSizeInBytes = mIndexSize * mIndexNum;
	if (isUseShadowBuffer)
	{
		mShadowHardwareBuffer = new BWIndexShadowHardwareBuffer(manager, mIndexType, mIndexNum, usage, isUseSystemMemory);
	}
}

size_t BWHardwareIndexBuffer::getIndexSize() const
{
	return mIndexSize;
}
BWHardwareIndexBuffer::IndexType BWHardwareIndexBuffer::getIndexType() const
{
	return mIndexType;
}