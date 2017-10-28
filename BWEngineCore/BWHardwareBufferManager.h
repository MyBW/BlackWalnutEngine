#ifndef HARDWAREBUFFERMANAGER_H_
#define HARDWAREBUFFERMANAGER_H_
#include "BWSingleton.h"
#include "BWHardwareIndexBuffer.h"
#include "BWHardwareVertexBuffer.h"
class BWHardwareBufferManagerBase
{
public:
	typedef std::list<BWHardwareIndexBuffer*> HardwareIndexBufferList;   // 这两个都是用的是指针 而不是智能指针 合理么？
	typedef std::list<BWHardwareVertexBuffer*> HardwareVertexBufferList;
	virtual BWHardwareIndexBufferPtr createIndexBuffer(const std::string &Name , BWHardwareIndexBuffer::IndexType type, size_t numIndexs,
	BWHardwareBuffer::Usage usage, bool useShadowBuffer = false) = 0;
	virtual BWHardwareVertexBufferPtr createVertexBuffer(const std::string &Name, unsigned int size, unsigned int count,
		BWHardwareBuffer::Usage usage, bool useShadowBuffer = false) = 0;
protected:
	HardwareIndexBufferList mHardwareIndexBufferList;
	HardwareVertexBufferList mHardwareVertexBufferList;
};


class BWHardwareBufferManager : public BWHardwareBufferManagerBase , BWSingleton<BWHardwareBufferManager>
{
public:
	BWHardwareBufferManager(BWHardwareBufferManagerBase *managerImp);
	static BWHardwareBufferManager* GetInstance();
    BWHardwareVertexBufferPtr createVertexBuffer(const std::string& Name, unsigned int size, unsigned int count,
		BWHardwareBuffer::Usage usage, bool useShadowBuffer /* = false */);
	BWHardwareIndexBufferPtr createIndexBuffer(const std::string& Name, BWHardwareIndexBuffer::IndexType type, size_t numIndexs, 
		BWHardwareBuffer::Usage usage, bool useShadowBuffer /* = false */);
protected:
	static BWHardwareBufferManager* instance;
	BWHardwareBufferManagerBase *Imp;
private:
};

#endif