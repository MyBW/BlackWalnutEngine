#ifndef GLHARDWAREVERTEXBUFFER_H_
#define GLHARDWAREVERTEXBUFFER_H_
#include "../BWEngineCore/BWHardwareVertexBuffer.h"
#include "GL/include/glew.h"
class GLHardwareVertexBuffer : public BWHardwareVertexBuffer
{
public:
	GLHardwareVertexBuffer(BWHardwareBufferManagerBase *manager,const std::string &Name, size_t vertexSize, size_t vertexNum,
		Usage usage, bool isSystemMemory, bool isUseShadowBuffer);
	GLuint getGLHardwareBufferID() const;

protected:
	void* lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option);
	void unLockImp();
	void updateFromShadowBuffer();
private:
	GLuint mBufferID;

};
#endif