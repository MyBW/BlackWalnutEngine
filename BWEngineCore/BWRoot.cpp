#include "BWRoot.h"
#include "../RenderSystem_GL/GLRenderSystem.h"
#include "OgreWindowEventUtilities.h"
#include "BWSceneManager.h"
BWRoot *BWRoot::GetInstance()
{
	assert(instance);
	return instance;
}

BWRoot::BWRoot() : BWSingleton()
,mActiveRenderSystem(NULL)
, mQuit(false)
{
	mIsInitialised = false;
}
BWRoot::~BWRoot()
{

}
BWRenderWindow* BWRoot::initialise(bool autoCreateWindow, const std::string &windowTitile /* = "Black Walnut Render Window" */, const std::string &customCapabilitiesConfig /* = StringUtil::BLANK */)
{
	if (customCapabilitiesConfig != StringUtil::BLANK)
	{
		assert(0);
	}
	// Init Render System
	mActiveRenderSystem = new GLRenderSystem();
	mRenderWindow = mActiveRenderSystem->_initialise(autoCreateWindow, windowTitile);
	mRenderWindow->_setPrimary();
	mIsInitialised = true;
	//Init SceneManaget
	mSceneManager = new BWSceneManager();
	return mRenderWindow;
}
BWRenderWindow* BWRoot::getFinalWindow()
{
	assert(mRenderWindow);
	return mRenderWindow;
}
bool BWRoot::InitRenderWindow()
{
	mRenderWindow->InitWindow();
	return true;
}
void BWRoot::clearEventTime()
{
	FrameStartTime = 0.0;
	FrameEndTime = 0.0;
	FrameDuringTime = 0.0;

}
void BWRoot::startRendering()
{
	assert(mActiveRenderSystem);
	mActiveRenderSystem->_initRenderTargets();
	clearEventTime();
	mQuit = false;
	while (!mQuit)
	{
		WindowEventUtilities::messagePump();
	    if (!renderOneFrame())
		{
			break;
		}
	}
}
bool BWRoot::renderOneFrame()
{
	if (!_fireFrameStarted())
	{
		return false;
	}
	if (!_updateAllRenderTarget())
	{
		return false;
	}
	return _fireFrameEnded();
}
bool BWRoot::_updateAllRenderTarget()
{
	//这里不转换buffer
	mActiveRenderSystem->_updateAllRenderTargets(false);

	bool ret = _fireFrameRenderingQueued();
	//在这里统一转换
	mActiveRenderSystem->_swapAllRenderTargetBuffers(mActiveRenderSystem->getWaitForVerticalBlank());

	//这里有一个场景管理器枚举工厂的更新 不知道搞啥用的

	return ret;
}

BWRenderSystem *BWRoot::getRenderSystem()
{
	return mActiveRenderSystem;
}
BWSceneManager* BWRoot::getSceneManager()
{
	return mSceneManager;
}
BWSceneManager* BWRoot::createSceneManager()
{
	assert(0);
	return mSceneManager;
}
bool BWRoot::_fireFrameEnded()
{
	FrameEndTime = clock();
	FrameDuringTime = (double)(FrameEndTime - FrameStartTime) / CLOCKS_PER_SEC;

	std::string FPS = "FPS : ";
	FPS += std::to_string(int(1.0/FrameDuringTime));
	BWLOG(FPS);
	return true;
}
bool BWRoot::_fireFrameRenderingQueued()
{
	return true;
}
bool BWRoot::_fireFrameStarted()
{
	FrameStartTime = clock();
	return true;
}
