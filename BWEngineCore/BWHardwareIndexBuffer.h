#ifndef HARDWAREINDEXBUFFER_H_
#define HARDWAREINDEXBUFFER_H_
#include "BWHardwareBuffer.h"
#include "BWSmartPointer.h"
class BWHardwareBufferManagerBase;
class BWHardwareIndexBuffer : public BWHardwareBuffer
{
public:
	enum IndexType
	{
		IT_16BIT ,
		IT_32BIT
	};
	BWHardwareIndexBuffer(BWHardwareBufferManagerBase *manager, const std::string& Name, IndexType indxType, size_t numberIndex,
		Usage  usage , bool isUseSystemMemory , bool isUseShadowBuffer);
	size_t getIndexSize() const;
	IndexType getIndexType() const;
protected:
private:
	IndexType mIndexType;
	size_t mIndexNum;
	size_t mIndexSize;
};
typedef SmartPointer<BWHardwareIndexBuffer> BWHardwareIndexBufferPtr;
#endif