#ifndef GLRENDERTARGET_H_
#define GLRENDERTARGET_H_
#include "../BWEngineCore/BWRenderTarget.h"
#include "GL/include/glew.h"
#include "GLVAO.h"
class GLRenderTarget : public BWRenderTarget
{
public:
	GLRenderTarget();
	~GLRenderTarget();
	void getCustomAttribute(const std::string &name, void *data);
	void copyContentsToMemory(const BWPixelBox &dst, FrameBuffer buffer);
	GLuint getID() const ;
	GLuint getColorAttachNum();
	GLenum getColorAttachment();
	void CollectAttachment(GLenum ColorAttachment);
	virtual bool requiresTextureFlipping() const;	
	void setDrawBuffers(const  NameLocation &BufferNameLocation);
	virtual void clearRenderTarget() override;
    void CopyToScreenFromColorBuffer(const std::string& ColorBufferName) override;
	void Destroty() override; 
	
protected:
	virtual BWHardwarePixelBufferPtr _createHardwarePixelBufferImp(const std::string &name, RenderBufferType bufferType, BWRenderTarget *creator) override;
	virtual BWHardwareStencilBufferPtr _createHardwareStencilBufferImp(const std::string &name, BWRenderTarget *creator) override;
	virtual BWHardwareDepthBufferPtr _createHardwareDepthBufferImp(const std::string &name, BWRenderTarget *creator) override;

	virtual bool _removeHardwarePixelBufferImp(BWHardwarePixelBufferPtr pixelBuffer) override;
	virtual bool _removeHardwareDepthBufferImp(BWHardwareDepthBufferPtr depthBuffer) override;
	virtual bool _removeHardwareStencilBufferImp(BWHardwareStencilBufferPtr stencilBuffer) override;
	virtual bool _removeTextureBufferImp(BWTexturePtr texture) override;
	void _setRenderTarget() override;
	void _removeRenderTarget() override;
	void _endUpdate() override;
	void _activeColorRenderBuffer();
private:
	GLuint mFrameBufferID;
	GLuint mDefultDepthBuffer;
	GLuint mColorAttachNum;
	GLuint mColorAttachMask;
    
	static GLint Max_Color_Attachment_Number;
};

#endif