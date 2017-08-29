#include "BWViewport.h"

BWViewport::BWViewport() :Zorder(0), mRenderTarget(NULL), mCamera(NULL)
{

}
BWViewport::BWViewport(BWCamera *camera, BWRenderTarget *target, float left, float top, float width, float height, float zOrder) :
mRenderTarget(target),
mCamera(camera),
mRelLeft(left),
mRelTop(top),
mRelWidth(width),
mRelHeight(height),
mZOrder(zOrder)
{
	_updateDimensions();
}
void BWViewport::update()
{
	if (mCamera)
	{
		mCamera->_renderScene(this, mShowOverlays);
	}
	return;
}
unsigned int BWViewport::_getNumRenderedBatches()
{
	return 0;
}
unsigned int BWViewport::_getNumRenderedFace()
{
	return 0;
}
bool BWViewport::_isUpdated() const 
{
	return true;
}
bool BWViewport::getClearEveryFrame()
{
	return true;
}

bool BWViewport::isAutoUpdated()
{
	return true;
}
BWRenderTarget* BWViewport::getRenderTarget()
{
	return mRenderTarget;
}

const ColourValue& BWViewport::getBackGroundColour() 
{
	return ColourValue::Black;
}

unsigned int BWViewport::getClearFrameBuffer()
{
	return 1000;
}
int BWViewport::getActualHeight() const 
{
	return mActHeight;
}
int BWViewport::getActualWidth() const
{
	return mActWidth;
}
int BWViewport::getActualLeft() const
{
	return mActLeft;
}
int BWViewport::getActualTop() const
{
	return mActTop;
}
void BWViewport::setMaterialScheme(const std::string &schemeName)
{
	mMaterialSchemeName = schemeName;
}
const std::string& BWViewport::getMaterialScheme()
{
	return mMaterialSchemeName;
}
void  BWViewport::_clearUpdateFlag()
{
	mUpdate = false; 
}
void BWViewport::_updateDimensions()
{
	float height = mRenderTarget->getHeight();
	float width = mRenderTarget->getWidth();
	mActLeft = (int)(mRelLeft * width);
	mActTop = (int)(mRelTop * height);
	mActWidth = (int)(mRelWidth * width);
	mActHeight = (int)(mRelHeight * height);
	if (mCamera)
	{
		if (mCamera->getAutoAspectRatio())
		{
			mCamera->setAspectRatio(float(mActWidth) / float(mActHeight));
		}
	}
	mUpdate = true;
}