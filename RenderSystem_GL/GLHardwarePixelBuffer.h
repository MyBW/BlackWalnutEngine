//#ifndef GLHARDWAREPIXELBUFFER_H_
//#define GLHARDWAREPIXELBUFFER_H_
//#include "../BWEngineCore/BWHardwarePixelBuffer.h"
//#include "GL/include/glew.h"
//class  GLHardwarePixelBuffer : public BWHardwarePixelBuffer
//{
//protected:
//	/// Lock a box
//	BWPixelBox lockImp(const BWBox lockBox, LockOptions options);
//
//	/// Unlock a box
//	void unLockImp(void);
//
//	// Internal buffer; either on-card or in system memory, freed/allocated on demand
//	// depending on buffer usage
//	BWPixelBox mBuffer;
//	GLenum mGLInternalFormat; // GL internal format
//	LockOptions mCurrentLockOptions;
//
//	// Buffer allocation/freeage
//	void allocateBuffer();
//	void freeBuffer();
//	// Upload a box of pixels to this buffer on the card
//	virtual void upload(const BWPixelBox &data, const BWBox &dest);
//	// Download a box of pixels from the card
//	virtual void download(const BWPixelBox &data);
//public:
//	/// Should be called by HardwareBufferManager
//	GLHardwarePixelBuffer(size_t mWidth, size_t mHeight, size_t mDepth,
//		PixelFormat mFormat,
//		BWHardwareBuffer::Usage usage);
//	~GLHardwarePixelBuffer();
//	/// @copydoc HardwarePixelBuffer::blitFromMemory
//	void blitFromMemory(const BWPixelBox &src, const BWBox &dstBox);
//
//	/// @copydoc HardwarePixelBuffer::blitToMemory
//	void blitToMemory(const BWBox &srcBox, const BWPixelBox &dst);
//
//	/** Bind surface to frame buffer. Needs FBO extension.
//	*/
//	virtual void bindToFramebuffer(GLenum attachment, size_t zoffset);
//	GLenum getGLFormat() { return mGLInternalFormat; }
//};
//
///** Texture surface.
//*/
//class  GLTextureBuffer : public GLHardwarePixelBuffer
//{
//public:
//	/** Texture constructor */
//	GLTextureBuffer(const std::string &baseName, GLenum target, GLuint id, GLint face,
//		GLint level, Usage usage, bool softwareMipmap, bool writeGamma, unsigned int fsaa);
//	~GLTextureBuffer();
//
//	/// @copydoc HardwarePixelBuffer::bindToFramebuffer
//	virtual void bindToFramebuffer(GLenum attachment, size_t zoffset);
//	/// @copydoc HardwarePixelBuffer::getRenderTarget
//	//RenderTexture* getRenderTarget(size_t);
//	/// Upload a box of pixels to this buffer on the card
//	virtual void upload(const BWPixelBox &data, const BWBox &dest);
//	// Download a box of pixels from the card
//	virtual void download(const BWPixelBox &data);
//
//	/// Hardware implementation of blitFromMemory
//	virtual void blitFromMemory(const BWPixelBox &src_orig, const BWBox &dstBox);
//	virtual bool attachToRenderTarget(BWRenderTarget *renderTarget) override { return false; }
//	/// Notify TextureBuffer of destruction of render target
//	void _clearSliceRTT(size_t zoffset)
//	{
//		//mSliceTRT[zoffset] = 0;
//	}
//	/// Copy from framebuffer
//	void copyFromFramebuffer(size_t zoffset);
//	/// @copydoc HardwarePixelBuffer::blit
//	void blit(const BWHardwarePixelBufferPtr &src, const BWBox &srcBox, const BWBox &dstBox);
//	// Blitting implementation
//	void blitFromTexture(GLTextureBuffer *src, const BWBox &srcBox, const BWBox &dstBox);
//protected:
//
//	// In case this is a texture level
//	GLenum mTarget;
//	GLenum mFaceTarget; // same as mTarget in case of GL_TEXTURE_xD, but cubemap face for cubemaps
//	GLuint mTextureID;
//	GLint mFace;
//	GLint mLevel;
//	bool mSoftwareMipmap;		// Use GLU for mip mapping
//
//	/*typedef vector<RenderTexture*>::type SliceTRT;
//	SliceTRT mSliceTRT;*/
//};
///** Renderbuffer surface.  Needs FBO extension.
//  感觉这个类应该放到 GLRenderTexture.h 中  然后 GLRenderTexture.h 改为 GLRenderTarget.h
//*/
////class  GLRenderBuffer : public GLHardwarePixelBuffer
////{
////public:
////	GLRenderBuffer(GLenum format, size_t width, size_t height, GLsizei numSamples);
////	~GLRenderBuffer();
////
////	/// @copydoc GLHardwarePixelBuffer::bindToFramebuffer
////	virtual void bindToFramebuffer(GLenum attachment, size_t zoffset);
////protected:
////	// In case this is a render buffer
////	GLuint mRenderbufferID;
////};
//#endif // !1
//
//
