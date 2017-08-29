#include "BWHardwareBufferManager.h"

BWHardwareBufferManager* BWHardwareBufferManager::instance = NULL;
BWHardwareBufferManager* BWHardwareBufferManager::GetInstance()
{
	assert(instance);
	return instance;
}
BWHardwareBufferManager::BWHardwareBufferManager(BWHardwareBufferManagerBase *managerImp) :Imp(managerImp)
{
	instance = this;
}

BWHardwareIndexBufferPtr BWHardwareBufferManager::createIndexBuffer(BWHardwareIndexBuffer::IndexType type, size_t numIndexs,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer)
{
	return Imp->createIndexBuffer(type, numIndexs, usage, useShadowBuffer);
}
BWHardwareVertexBufferPtr BWHardwareBufferManager::createVertexBuffer(unsigned int size, unsigned int count,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer)
{
	return Imp->createVertexBuffer(size, count, usage, useShadowBuffer);
}