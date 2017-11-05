#ifndef ROOT_H_
#define ROOT_H_
#include "BWSingleton.h"
#include "BWRenderSystem.h"
#include "BWRenderTarget.h"
#include <time.h>
class BWRoot : public BWSingleton<BWRoot>
{
	
public:

	BWRoot();
	~BWRoot();
	BWRenderWindow* initialise(bool autoCreateWindow, const std::string &windowTitile = "Black Walnut Render Window",
		const std::string &customCapabilitiesConfig = StringUtil::BLANK);
	void startRendering();
	void clearEventTime();
	bool renderOneFrame();
	
	BWRenderSystem *getRenderSystem();
	BWSceneManager* getSceneManager();
	BWSceneManager* createSceneManager();
	BWRenderWindow* getFinalWindow();
	bool  InitRenderWindow();
	static BWRoot *GetInstance();
	double GetDeltaTime() const { return FrameDuringTime; }
	BWRenderSystem *mActiveRenderSystem;
private:
	bool _fireFrameStarted();// 一帧渲染开始前
	bool _fireFrameEnded();  // 一帧渲染结束后
	bool _fireFrameRenderingQueued();//一帧渲染过程中 利用的是GPU时间
	bool _updateAllRenderTarget();

	bool mQuit;
	bool mIsInitialised;
	
	///////
	clock_t FrameStartTime;
	clock_t FrameEndTime;
	double FrameDuringTime{ 0.0f };


	BWRenderWindow *mRenderWindow;
	BWSceneManager *mSceneManager;
};
#endif
