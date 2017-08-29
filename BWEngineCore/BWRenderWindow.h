#ifndef RENDERWINDOW_H_
#define RENDERWINDOW_H_
#include "BWRenderTarget.h"
#include "BWCommon.h"
//�����ڳ����༭�����ж����Ⱦ���� ����һ��������
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
	// �����㲻�ڸ�window��ʱ  ��window�Ƿ��ʧЧ
	void setDeactivateOnFocusChange(bool deactivate);
	void windowMovedOrResized(){ }
	
	virtual bool InitWindow() = 0;
protected: 
	bool mFullScreen;
	bool mIsPrimary;
	bool mAutoDeactivateOnFocusChange;
	int mleft;
	int mtop;
	// ˵��������������window ֻ����root��ʹ��
	void _setPrimary() { mIsPrimary = true; }
	friend class BWRoot;
private:

};


#endif