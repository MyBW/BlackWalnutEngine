#ifndef SHADOWHARDWAREBUFFER_H_
#define SHADOWHARDWAREBUFFER_H_
#include "BWHardwareVertexBuffer.h"
#include "BWHardwareIndexBuffer.h"
class BWVertexShadowHardwareBuffer : public BWHardwareVertexBuffer
{
public:
	BWVertexShadowHardwareBuffer(BWHardwareBufferManagerBase *manager ,const std::string &Name, size_t vertexSize , size_t vertexNum ,
		Usage usage , bool isSystemMemory );
	~BWVertexShadowHardwareBuffer();
	void* lock(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void  unlock();
protected:
	void * lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void unLockImp();
private:
	unsigned char *mData;

};
class BWIndexShadowHardwareBuffer : public BWHardwareIndexBuffer
{
public:
	BWIndexShadowHardwareBuffer(BWHardwareBufferManagerBase * manager, const std::string& Name, IndexType indexType, size_t indexNum, Usage usage, bool isUseSystem);
	void *lock(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void unlock();
protected:
	void* lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void  unLockImp();
private: 
	unsigned char* mData;
};
#endif