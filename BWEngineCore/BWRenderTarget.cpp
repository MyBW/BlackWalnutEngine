#include "BWRenderTarget.h"
#include "BWRoot.h"
#include "BWViewport.h"
#include "BWLog.h"
#include "BWRenderSystem.h"
BWRenderTarget::BWRenderTarget() :renderTargetDes(BW_BUFFER_RENDERTARGET) ,
mIsHardwareGammaEnable(false),
mPriority(0)
{
}
BWRenderTarget::~BWRenderTarget()
{
	Destroty();
}

void BWRenderTarget::update(bool isSwapBuffer)
{
	updateImp();
	if (isSwapBuffer)
	{
		swapBuffers(BWRoot::GetInstance()->getRenderSystem()->getWaitForVerticalBlank());
	}
}
void BWRenderTarget::_beginUpdate()
{
	firePreUpdate();
	mFrameStats.triangleCount = 0;
	mFrameStats.batchCount = 0;
}
void BWRenderTarget::firePreUpdate()
{

}
void BWRenderTarget::fireViewportPreUpdate(BWViewport *viewport)
{

}
void BWRenderTarget::fireViewportAdded(BWViewport *viewport)
{

}
void BWRenderTarget::updateImp()
{
	_beginUpdate();
	_setRenderTarget();
	_updateAutoUpdatedViewports(true);
	_removeRenderTarget();
	_endUpdate();
}

void BWRenderTarget::Destroty()
{
	for (auto &Ele : mRenderBufferMap)
	{
		Ele.second->RemoveFromRenderTarget();
	}
	for (auto &Ele : mDepthBufferMap)
	{
		Ele.second->RemoveFromRenderTarget();
	}
	for (auto &Ele : mStencilBufferMap)
	{
		Ele.second->RemoveFromRenderTarget();
	}

	for (auto &Ele : mTextureBufferMap)
	{
		Ele.second->RemoveFromRenderTarget();
	}
	mRenderBufferMap.clear();
	mDepthBufferMap.clear();
	mStencilBufferMap.clear();
	mTextureBufferMap.clear();
}

void BWRenderTarget::_endUpdate()
{

}
bool BWRenderTarget::isActive()
{
	return true;
}
bool BWRenderTarget::isAutoUpdated()
{
	return true;
}
void BWRenderTarget::setPriority(unsigned char priority)
{
	mPriority = priority;
}
void BWRenderTarget::setActive(bool state)
{
	mActive = state;
}
void BWRenderTarget::_updateAutoUpdatedViewports(bool updateStatistics)
{
	Viewportlist::iterator itor = viewportPriorityList.begin();
	Viewportlist::iterator itorEnd = viewportPriorityList.end();
	for (; itor != itorEnd; itor++)
	{
		BWViewport *viewport = (*itor).second;
		if (viewport->isAutoUpdated())
		{
			_updateViewport(viewport, updateStatistics);
		}
	}
}
void BWRenderTarget::_updateViewport(BWViewport *viewport, bool updateStatistic)
{
	assert(viewport->getRenderTarget() == this);
	fireViewportPreUpdate(viewport);
	viewport->update();
	if (updateStatistic)
	{

		mFrameStats.triangleCount += viewport->_getNumRenderedFace();
		mFrameStats.batchCount += viewport->_getNumRenderedBatches();
	}
}
void BWRenderTarget::swapBuffers(bool waritForVSync /* = true */)
{
	waritForVSync;
}
void BWRenderTarget::resetStatistics()
{
	mFrameStats.avgFPS = 0.0;
	mFrameStats.bestFPS = 0.0;
	mFrameStats.worstFPS = 999.0;
	mFrameStats.triangleCount = 0;
	mFrameStats.batchCount = 0;
	mFrameStats.bestFrameTime = 999999;
	mFrameStats.worstFrameTime = 0;

	mLastTime = mTimer->getMilliseconds();
	mLastSecond = mLastTime;
	mFrameCount = 0.0;
}
float BWRenderTarget::getLastFPS() const
{
	return 1;
}

BWViewport* BWRenderTarget::getViewport(int index)
{
	return viewportPriorityList[index];
}

void BWRenderTarget::addViewport(BWCamera *camera ,  int zOrder , float left , float top , float width , float height)
{
	Viewportlist::iterator itor = viewportPriorityList.find(zOrder);
	if (itor != viewportPriorityList.end())
	{
		assert(0);
	}
	BWViewport *viewport = new BWViewport(camera, this, left, top, width, height, zOrder);
	viewportPriorityList[zOrder] = viewport;
	fireViewportAdded(viewport);
}
int BWRenderTarget::getPriority()
{
	return mPriority;
}
bool BWRenderTarget::isHardwareGammaEnabled() const
{
	return mIsHardwareGammaEnable;
}
void BWRenderTarget::setHardwareGammaEnabled(bool Gamma)
{
	mIsHardwareGammaEnable = Gamma;
}
unsigned int BWRenderTarget::getHeight() const
{
	return mHeight;
}
unsigned int BWRenderTarget::getWidth() const
{
	return mWidth;
}
void BWRenderTarget::setHeight(unsigned int height)
{
	mHeight = height;
}
void BWRenderTarget::setWidth(unsigned int width)
{
	mWidth = width;
}
bool BWRenderTarget::addTextureBuffer(BWTexturePtr texture , int Index , int MipLevel)
{
	if (mTextureBufferMap.find(texture->getName()) == mTextureBufferMap.end() 
		&& mRenderBufferMap.find(texture->getName())== mRenderBufferMap.end())
	{
		mTextureBufferMap[texture->getName()] = texture;
	}
	texture->attachToRenderTarget(this, Index, MipLevel);
	return true;
}
BWTexturePtr BWRenderTarget::getTextureBuffer(const std::string &name) const 
{
	TextureBufferMap::const_iterator itor = mTextureBufferMap.find(name);
	if (itor != mTextureBufferMap.end())
	{
		return itor->second;
	}
	return NULL;
}
bool BWRenderTarget::removeTextureBuffer(const std::string & name)
{
	TextureBufferMap::iterator result = mTextureBufferMap.find(name);
	if (result != mTextureBufferMap.end())
	{
		if (!_removeTextureBufferImp(result->second))
		{
			return false;
		}
		mTextureBufferMap.erase(result);
	}
	return true; 
}
bool BWRenderTarget::createPixelBuffer(std::string& name)
{
	BWHardwarePixelBufferPtr pixelBuffer = _createHardwarePixelBufferImp(name, TY_RenderBuffer , this);
	if (pixelBuffer.Get())
	{
		if (addPixelRenderBuffer(pixelBuffer))
		{
			return true;
		}
		Log::GetInstance()->logMessage("BWRenderTarget::createPixelBuffer can not add pixel buffer");
		return false;
	}
	Log::GetInstance()->logMessage("BWRenderTarget::createPixelBuffer can not create pixel buffer");
	return false;
}
bool BWRenderTarget::addPixelRenderBuffer(BWHardwarePixelBufferPtr renderBuffer)
{
	Log::GetInstance()->logMessage(renderBuffer->getName());
	if (mTextureBufferMap.find(renderBuffer->getName()) == mTextureBufferMap.end()
		&& mRenderBufferMap.find(renderBuffer->getName()) == mRenderBufferMap.end())
	{
		renderBuffer->attachToRenderTarget(this);
		mRenderBufferMap[renderBuffer->getName()] = renderBuffer;
		return true;
	}
	return false;
}
bool BWRenderTarget::removePixelRenderBuffer(std::string& name)
{
	PixelBufferMap::iterator result = mRenderBufferMap.find(name);
	if (result != mRenderBufferMap.end())
	{
		if (!_removeHardwarePixelBufferImp(result->second))
		{
			return false;
		}
		mRenderBufferMap.erase(result);
	}
	return true;
}
BWHardwarePixelBufferPtr BWRenderTarget::getPixelRenderBuffer(const std::string &name)
{
	PixelBufferMap::iterator result = mRenderBufferMap.find(name);
	if (result != mRenderBufferMap.end())
	{
		return result->second;
	}
	return NULL;
}
BWHardwareDepthBufferPtr BWRenderTarget::CreateDepthBufferWithoutAdd(const std::string &name)
{
	return _createHardwareDepthBufferImp(name, this);
}
BWHardwareDepthBufferPtr BWRenderTarget::createDepthBuffer(std::string& name)
{
	BWHardwareDepthBufferPtr depthBuffer = _createHardwareDepthBufferImp(name,this);
	if (depthBuffer.Get())
	{
		if (addDepthRenderBuffer(depthBuffer))
		{
			return depthBuffer;
		}
	}
	Log::GetInstance()->logMessage("BWRenderTarget::createDepthBuffer can not create depth buffer");
	return NULL;
}
bool BWRenderTarget::addDepthRenderBuffer(BWHardwareDepthBufferPtr renderBuffer)
{
	if (mDepthBufferMap.find(renderBuffer->getName()) == mDepthBufferMap.end())
	{
		renderBuffer->attachToRenderTarget(this);
		mDepthBufferMap[renderBuffer->getName()] = renderBuffer;
		return true;
	}
	return false;
}
bool BWRenderTarget::removeDpethRenderBuffer(const std::string& name)
{
	DepthBufferMap::iterator result = mDepthBufferMap.find(name);
	if (result != mDepthBufferMap.end())
	{
		if (!_removeHardwareDepthBufferImp(result->second))
		{
			return false;
		}
		mDepthBufferMap.erase(result);
	}
	return true;
}
BWHardwareDepthBufferPtr BWRenderTarget::getDepthRenderBuffer(std::string& name)
{
	if (mDepthBufferMap.find(name) != mDepthBufferMap.end())
	{
		return mDepthBufferMap[name];
	}
	return NULL;
}
bool BWRenderTarget::createStencilBuffer(std::string& name)
{
	BWHardwareStencilBufferPtr stencilBuffer = _createHardwareStencilBufferImp(name, this);
	if (stencilBuffer.Get())
	{
		if (addStencilRenderBuffer(stencilBuffer))
		{
			return true;
		}
		Log::GetInstance()->logMessage("BWRenderTarget::createStencilBuffer can not add stencil buffer");
		return false;
	}
	Log::GetInstance()->logMessage("BWRenderTarget::createStencilBuffer can not create stencil buffer");
	return false;
}
bool BWRenderTarget::addStencilRenderBuffer(BWHardwareStencilBufferPtr renderBuffer)
{
	if (mStencilBufferMap.find(renderBuffer->getName()) == mStencilBufferMap.end())
	{
		renderBuffer->attachToRenderTarget(this);
		mStencilBufferMap[renderBuffer->getName()] = renderBuffer;
		return true;
	}
	return false;
}
bool BWRenderTarget::removeStencilRenderBuffer(std::string &name)
{
	StencilBufferMap::iterator result = mStencilBufferMap.find(name);
	if (result != mStencilBufferMap.end())
	{
		if (!_removeHardwareStencilBufferImp(result->second))
		{
			return false;
		}
		mStencilBufferMap.erase(result);
	}
	return true;
}
//void BWRenderTarget::addRenderBuffer(BWRenderBuffer *renderBuffer)
//{
//	if (mRenderBufferMap.find(renderBuffer->getName()) == mRenderBufferMap.end())
//	{
//		renderBuffer->attachToRenderTarget(this);
//		mRenderBufferMap[renderBuffer->getName()] = renderBuffer;
//	}
//}
//void BWRenderTarget::addRenderBuffer(const std::string &name, RenderBufferType bufferType ,RenderBufferContentType contentType)
//{
//	assert(mRenderBufferMap.find(name) == mRenderBufferMap.end());
//	if (mRenderBufferMap.find(name) != mRenderBufferMap.end())
//	{
//		Log::GetInstance()->logMessage("BWRenderTarget::addRenderBuffer()");
//	}
//	BWRenderBuffer *buffer = _createRenderBufferImp(name , bufferType , contentType , this);
//	mRenderBufferMap[name] = buffer;
//}
//void BWRenderTarget::removeRenderBuffer(const std::string &name)
//{
//	RenderBufferMap::iterator itor = mRenderBufferMap.find(name);
//	if (itor != mRenderBufferMap.end())
//	{
//		BWRenderBuffer *buffer = itor->second;
//		_removeRenderTargetImp(buffer);
//		mRenderBufferMap.erase(itor);
//		delete buffer;
//	}
//}
//void BWRenderTarget::_removeRenderTargetImp(BWRenderBuffer *renderBuffer)
//{
//}
void BWRenderTarget::_setRenderTarget()
{

}
void BWRenderTarget::_removeRenderTarget()
{

}
BWHardwarePixelBufferPtr BWRenderTarget::_createHardwarePixelBufferImp(const std::string &name, RenderBufferType bufferType, BWRenderTarget *creator)
{
	return NULL;
}
BWHardwareStencilBufferPtr BWRenderTarget::_createHardwareStencilBufferImp(const std::string &name, BWRenderTarget *creator)
{
	return NULL;
}
BWHardwareDepthBufferPtr BWRenderTarget::_createHardwareDepthBufferImp(const std::string &name, BWRenderTarget *creator)
{
	return NULL;
}
//BWRenderBuffer* BWRenderTarget::_createRenderBufferImp(const std::string &name, RenderBufferType bufferType, RenderBufferContentType contentType, BWRenderTarget *creator)
//{
//	return NULL;
//}