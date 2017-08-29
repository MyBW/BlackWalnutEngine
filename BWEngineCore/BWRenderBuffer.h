//#ifndef RENDERBUFFER_H_
//#define RENDERBUFFER_H_
//
//#include "BWTexture.h"
//
//class BWRenderTarget;
//enum RenderBufferType
//{
//	TY_RenderBuffer,
//	TY_TextureBuffer
//};
//enum RenderBufferContentType
//{
//	CT_Color,
//	CT_Depth,
//	CT_Stencil,
//	CT_Stencil_Depth
//};

//class BWRenderBuffer
//{
//public:
//	BWRenderBuffer(unsigned int width, unsigned int height, PixelFormat format);
//	/*void setWidth(unsigned int width);
//	void setHeight(unsigned int height);
//	void setBitDepth(unsigned char bitDepth);*/
//	void setName(const std::string &name);
//	const std::string& getName() const;
//	void setContentType(RenderBufferContentType type);
//	RenderBufferContentType getContentType() const;
//	void setType(RenderBufferType type);
//	//RenderBufferType getType() const;
//	/*virtual void convertToTexture(BWTexturePtr texture);
//	virtual void convertToRenderBuffer(BWRenderBuffer& renderBuffer);
//	virtual void convertToMemoryBuffer(unsigned char *buffer);*/
//	virtual void attachToRenderTarget(BWRenderTarget *renderTarget) = 0;
//	virtual void removeFromRenderTarget() = 0;
//    BWTexturePtr getTexture() const; 
//protected:
//	BWRenderTarget *mRenderTarget;
//	std::string mName;
//	RenderBufferContentType mContentType;
//	//RenderBufferType  mBufferType;
//	unsigned int mWidth;
//	unsigned int mHeight;
//	PixelFormat mFormat;
//	BWTexturePtr mTexture;
//};
//
//#endif 