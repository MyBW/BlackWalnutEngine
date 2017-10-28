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

BWHardwareIndexBufferPtr BWHardwareBufferManager::createIndexBuffer(const std::string& Name, BWHardwareIndexBuffer::IndexType type, size_t numIndexs,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer)
{
	return Imp->createIndexBuffer(Name, type, numIndexs, usage, useShadowBuffer);
}
BWHardwareVertexBufferPtr BWHardwareBufferManager::createVertexBuffer(const std::string& Name, unsigned int size, unsigned int count,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer)
{
	return Imp->createVertexBuffer(Name ,size, count, usage, useShadowBuffer);
}