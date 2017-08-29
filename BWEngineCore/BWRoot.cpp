#include "BWRoot.h"
#include "../RenderSystem_GL/GLRenderSystem.h"
#include "OgreWindowEventUtilities.h"
#include "BWSceneManager.h"
BWRoot *BWRoot::GetInstance()
{
	assert(instance);
	return instance;
}

BWRoot::BWRoot() :mActiveRenderSystem(NULL)
, mQuit(false)
{
	mActiveRenderSystem = new GLRenderSystem();
	mSceneManager = new BWSceneManager();
	mIsInitialised = false;
}
BWRoot::~BWRoot()
{

}
BWRenderWindow* BWRoot::initialise(bool autoCreateWindow, const std::string &windowTitile /* = "Black Walnut Render Window" */, const std::string &customCapabilitiesConfig /* = StringUtil::BLANK */)
{
	if (!mActiveRenderSystem)
	{
		assert(0);
	}
	if (customCapabilitiesConfig != StringUtil::BLANK)
	{
		assert(0);
	}
	mRenderWindow = mActiveRenderSystem->_initialise(autoCreateWindow, windowTitile);
	mRenderWindow->_setPrimary();
	mIsInitialised = true;
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
	//���ﲻת��buffer
	mActiveRenderSystem->_updateAllRenderTargets(false);

	bool ret = _fireFrameRenderingQueued();
	//������ͳһת��
	mActiveRenderSystem->_swapAllRenderTargetBuffers(mActiveRenderSystem->getWaitForVerticalBlank());

	//������һ������������ö�ٹ����ĸ��� ��֪����ɶ�õ�

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
	return true;
}
bool BWRoot::_fireFrameRenderingQueued()
{
	return true;
}
bool BWRoot::_fireFrameStarted()
{
	return true;
}
