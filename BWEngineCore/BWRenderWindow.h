#ifndef RENDERWINDOW_H_
#define RENDERWINDOW_H_
#include "BWRenderTarget.h"
#include "BWCommon.h"
//例如在场景编辑器中有多个渲染窗口 这是一个抽象类
class BWRenderWindow :  public BWRenderTarget
{
public:
	BWRenderWindow();
	virtual ~BWRenderWindow();
	virtual void create(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams = 0) = 0;
	virtual void setFullScreen(bool fullScreen, unsigned int width, unsigned int height)
	{

	}
	virtual void destroy() = 0;
	virtual void resize(unsigned int width, unsigned int height) = 0;
	virtual void windowMoveOrReisze(){ }
	virtual void rePosition(int left, int top) = 0;
	virtual bool isVisible() const { return true; }
	virtual void setVisisble(bool visible) {  }
	virtual bool isActive() const { return mActive && isVisible(); }
	virtual bool isClosed() const = 0;
	virtual bool isPrimary() const { return mIsPrimary; }
	virtual void getMetrics(unsigned int &width, unsigned int &height, unsigned int & colorDepth,
		int &left, int &top);
	bool isDeactivateOnFocusChange() const;
	// 当焦点不在该window上时  该window是否会失效
	void setDeactivateOnFocusChange(bool deactivate);
	void windowMovedOrResized(){ }
	
	virtual bool InitWindow() = 0;
protected: 
	bool mFullScreen;
	bool mIsPrimary;
	bool mAutoDeactivateOnFocusChange;
	int mleft;
	int mtop;
	// 说明这个是最基本的window 只能由root来使用
	void _setPrimary() { mIsPrimary = true; }
	friend class BWRoot;
private:

};


#endif