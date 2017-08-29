#ifndef GLHARDWAREBUFFERMANAGER_H_
#define GLHARDWAREBUFFERMANAGER_H_
#include "../BWEngineCore/BWHardwareBufferManager.h"
#include "GL/include/glew.h"
 class GLHardwareBufferManager : public BWHardwareBufferManagerBase
 {
 public:
	 static GLenum getGLUsage(unsigned int usage);
	 static GLenum getGLType(unsigned int type);
	 BWHardwareIndexBufferPtr createIndexBuffer(BWHardwareIndexBuffer::IndexType type, size_t numIndexs,
		 BWHardwareBuffer::Usage usage, bool useShadowBuffer);
	 BWHardwareVertexBufferPtr createVertexBuffer(unsigned int size, unsigned int count,
		 BWHardwareBuffer::Usage usage, bool useShadowBuffer);
 protected:
 private:
 };
#endif