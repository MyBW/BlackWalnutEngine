#ifndef GLHARDWAREDEPTHBUFFER_H_
#define GLHARDWAREDEPTHBUFFER_H_
#include "../BWEngineCore/BWHardwareDepthBuffer.h"
#include "GL/include/glew.h"
class GLHardwareDepthBuffer :public BWHardwareDepthBuffer
{
public:
	GLHardwareDepthBuffer(const std::string &name, size_t width, size_t height, size_t depth,
		BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
	~GLHardwareDepthBuffer();

	virtual bool attachToRenderTarget(BWRenderTarget* renderTarget) override;
	void RemoveFromRenderTarget() override;
	GLenum GetAttachment() { return mAttachment; }
	void SetIsWithStencil(bool IsWithStencil) override;
	GLuint GetHIID() { return mDepthBufferID; }
	FORCEINLINE GLenum GetInternalFormat() { return mInternalFormat; }
protected:
	virtual void* lockImp(size_t offset, size_t length, BWHardwareBuffer::LockOptions option) override;
	virtual void  unLockImp() override;
	void SetBufferSizeImp(int Width, int Heigh) override;
	void SetBufferMipmapImp(int InMipmapLevelNum) override;
private:
	GLuint mDepthBufferID;
	GLenum mInternalFormat;
	GLenum mAttachment;
};

#endif