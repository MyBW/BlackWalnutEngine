#ifndef GLHARDWAREINDEXBUFFER_H_
#define GLHARDWAREINDEXBUFFER_H_
#include "../BWEngineCore/BWHardwareBufferManager.h"
#include"../BWEngineCore/BWHardwareIndexBuffer.h"
#include "GL/include/glew.h"

class GLHardwareIndexBuffer : public BWHardwareIndexBuffer
{
public:
	GLHardwareIndexBuffer(BWHardwareBufferManagerBase *manager , const std::string& Name, IndexType indexType ,  size_t indexNum , Usage usage , bool isUseSystemBuffer , bool isUseShadowBuffer);
	~GLHardwareIndexBuffer();
	GLuint getGLBufferID() const;
protected:
	void *lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void unLockImp();
	void updateFromShadowBuffer();
protected:
	GLuint mBufferID;
};

#endif
