#ifndef GLHARDWARESTENCILBUFFER_H_
#define GLHARDWARESTENCILBUFFER_H_
#include "../BWEngineCore/BWHardwareStencilBuffer.h"
#include "GL/include/glew.h"
class BWRenderTarget;

class GLHardwareStencilBuffer : public BWHardwareStencilBuffer
{
public:
	GLHardwareStencilBuffer( const std::string &name, size_t width, size_t height, size_t depth,
		BWRenderTarget* renderTarget, BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
	~GLHardwareStencilBuffer();
	virtual bool attachToRenderTarget(BWRenderTarget* renderTarget) override;
	void RemoveFromRenderTarget()override;
protected:
	virtual void* lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option) override;
	virtual void  unLockImp() override;
private:
	GLuint mStencilBufferID;
	GLenum mInternalFormat;
	GLenum mAttachment;
};




#endif