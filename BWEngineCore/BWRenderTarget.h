#ifndef RENDER_TARGET_H_
#define RENDER_TARGET_H_
#include<map>
#include "BWPrimitive.h"
#include "BWTimer.h"
#include "BWRenderBuffer.h"
#include "BWHardwarePixelBuffer.h"
#include "BWHardwareStencilBuffer.h"
#include "BWHardwareDepthBuffer.h"
#include "BWTexture.h"
class BWViewport;
class BWCamera;
enum  RenderTarget_Des
{
   BW_TEXTURE_RENDERTARGET,
   BW_BUFFER_RENDERTARGET
};

class BWRenderTarget
{
public: 
	enum StatFlags
	{
		SF_NONE = 0 ,
		SF_FPS  = 1 ,
		SF_AVG_FPS = 2 ,
		SF_BEST_FPS = 3 ,
		SF_WORST_FPS = 4 ,
		SF_TRIANGLE_COUNT = 5,
		SF_ALL = 0xFFFF
	};
	struct FrameStats 
	{
		float lastFPS;
		float avgFPS;
		float bestFPS;
		float worstFPS;
		unsigned long bestFrameTime;
		unsigned long worstFrameTime;
		size_t triangleCount;
		size_t batchCount;
	};
	enum FrameBuffer
	{
		FB_FRONT,
		FB_BACK ,
		FB_AUTO
	};
	
	typedef std::map<std::string, BWHardwarePixelBufferPtr> PixelBufferMap;
	typedef std::map<std::string, BWHardwareStencilBufferPtr> StencilBufferMap;
	typedef std::map<std::string, BWHardwareDepthBufferPtr> DepthBufferMap;
	typedef std::map<std::string, BWTexturePtr> TextureBufferMap;
	// π”√zOrder∂‘viewport≈≈–Ú
	typedef std::map<int, BWViewport*> Viewportlist;

	BWRenderTarget();
	virtual ~BWRenderTarget();

	size_t getHeight() const;
	size_t getWidth() const ;
	void setHeight(unsigned int height);
	void setWidth(unsigned int width);

	bool isActive();
	virtual void setActive(bool state);
	bool isAutoUpdated();
	void setPriority(unsigned char);
	void update(bool isSwapBuffer);
	void resetStatistics();
	void swapBuffers(bool waritForVSync = true);
	void _updateViewport(BWViewport *viewport, bool updateStatistic);
	virtual void firePreUpdate();
	virtual void fireViewportPreUpdate(BWViewport *viewport);
	virtual void fireViewportAdded(BWViewport *viewport);
	virtual bool requiresTextureFlipping() const = 0;
	float getLastFPS() const;
	virtual void copyContentsToMemory(const BWPixelBox &dst, FrameBuffer buffer) = 0;
	virtual PixelFormat suggestPixelFormat() const { return PF_BYTE_RGBA; }
	BWViewport *getViewport(int index);
	virtual void addViewport(BWCamera *camera , int zOrder = 0 , float left = 0 , float top = 0, float width = 1 , float height = 1);
	bool isHardwareGammaEnabled() const;
	void setHardwareGammaEnabled(bool Gamma);
	int getPriority();
	const std::string& getName(){ return mName; }
	virtual void getCustomAttribute(const std::string &name, void *data) = 0;

	/*virtual void addRenderBuffer(BWRenderBuffer *renderBuffer);
	virtual void addRenderBuffer(const std::string &name , RenderBufferType renderBufferType ,RenderBufferContentType constentType);
	virtual void removeRenderBuffer(const std::string &name);*/

	virtual bool addTextureBuffer(BWTexturePtr texture , int Index , int MipLevel =0);
	virtual bool removeTextureBuffer(const std::string & name);
	BWTexturePtr getTextureBuffer(const std::string &name) const ;

	bool createPixelBuffer(std::string&  name);
	virtual bool addPixelRenderBuffer(BWHardwarePixelBufferPtr renderBuffer);
	virtual bool removePixelRenderBuffer(std::string& name);

	BWHardwareDepthBufferPtr createDepthBuffer(std::string& name);
	BWHardwareDepthBufferPtr CreateDepthBufferWithoutAdd(const std::string &name);
	virtual bool addDepthRenderBuffer(BWHardwareDepthBufferPtr depthRenderBuffer);
	virtual bool removeDpethRenderBuffer(const std::string& name);
	BWHardwareDepthBufferPtr getDepthRenderBuffer(std::string& name);
	BWHardwarePixelBufferPtr getPixelRenderBuffer(const std::string &name);
	bool createStencilBuffer(std::string& name);
	virtual bool addStencilRenderBuffer(BWHardwareStencilBufferPtr stencilRenderBuffer);
	virtual bool removeStencilRenderBuffer(std::string &name);

	virtual void clearRenderTarget() = 0;
	virtual void CopyToScreenFromColorBuffer(const std::string& ColorBufferName ) { };
	bool hasDepthRenderBuffer() { return mDepthBufferMap.size() != 0; }

	virtual void updateImp();
	virtual void Destroty();
	virtual void _beginUpdate();
	virtual void _setRenderTarget();
	virtual void _updateAutoUpdatedViewports(bool updateStatistics);
	virtual void _removeRenderTarget();
	virtual void _endUpdate();
protected:


	virtual BWHardwarePixelBufferPtr _createHardwarePixelBufferImp(const std::string &name, RenderBufferType bufferType, BWRenderTarget *creator) = 0;
	virtual BWHardwareStencilBufferPtr _createHardwareStencilBufferImp(const std::string &name, BWRenderTarget *creator) = 0 ;
	virtual BWHardwareDepthBufferPtr _createHardwareDepthBufferImp(const std::string &name, BWRenderTarget *creator) = 0;
	virtual bool _removeHardwarePixelBufferImp(BWHardwarePixelBufferPtr pixelBuffer) = 0;
	virtual bool _removeHardwareDepthBufferImp(BWHardwareDepthBufferPtr depthBuffer) = 0;
	virtual bool _removeHardwareStencilBufferImp(BWHardwareStencilBufferPtr stencilBuffer)= 0;
	virtual bool _removeTextureBufferImp(BWTexturePtr texture) = 0;

	PixelBufferMap mRenderBufferMap;
	DepthBufferMap mDepthBufferMap;
	StencilBufferMap mStencilBufferMap;
	TextureBufferMap mTextureBufferMap;

	Viewportlist viewportPriorityList;
	RenderTarget_Des renderTargetDes;
	bool mIsDepthBuffered;
	int mColourDepth;
	std::string mName;
	unsigned char mPriority;

	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mColorDepth;

	FrameStats mFrameStats;
	BWTimer * mTimer;
	unsigned long mLastSecond;
	unsigned long mLastTime;
	size_t mFrameCount;
	bool mActive;
	bool mHwGamma;
	unsigned int mFSAA;
	std::string mFSAAHint;

	bool mIsHardwareGammaEnable;
};

#endif