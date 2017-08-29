#ifndef GLHardwarePixelBuffer_H_
#define GLHardwarePixelBuffer_H_
#include "GL/include/glew.h"
#include "../BWEngineCore/BWHardwarePixelBuffer.h"
class GLHardwarePixelBuffer : public BWHardwarePixelBuffer
{
public:
	GLHardwarePixelBuffer(const std::string &name ,unsigned int width , unsigned int  height , PixelFormat format , 
		 BWRenderTarget *creator);
	~GLHardwarePixelBuffer();
	virtual bool attachToRenderTarget(BWRenderTarget *renderTarget) override;
	virtual void removeFromRenderTarget();
	void RemoveFromRenderTarget() override;
	GLenum getAttachment() const;

	virtual void unLockImp() override;
	virtual BWPixelBox lockImp(const BWBox lockBox, LockOptions option) override;
	virtual void blitFromMemory(const BWPixelBox &src, const BWBox &desBox) override;
	virtual void blitToMemory(const BWBox &srcBox, const BWPixelBox &des)override;

private: 
	GLuint mRenderbufferID;
	GLenum mInternalFormat;
	GLenum mAttachment;
};

#endif