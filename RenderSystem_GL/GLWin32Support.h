#ifndef WIN32SUPPORT_H_
#define WIN32SUPPORT_H_
#include <windows.h>
#include "GLSupport.h"
class WIn32Window;
class GLWin32Support : public GLSupport
{
public:
	struct DisplayMonitorInfo
	{
		HMONITOR hMonitor;
		MONITORINFOEX monitorInfoEX;
	};
	GLWin32Support();
	~GLWin32Support();
	void initialiseWGL();
	void start();
	bool selectPixelFormat(HDC hdc, int colourDepht, int multiSample, bool hwGamma);
	virtual void initialiseExtension();
	static LRESULT CALLBACK dummyWndProc(HWND hwnd, unsigned int umsg, WPARAM wp, LPARAM lp);
	static BOOL CALLBACK sCreateMonitorsInfoEnumProc(HMONITOR Hmonitor, HDC hdcMonitor,
		LPRECT lprcMonitor, LPARAM dwData);
	BWRenderWindow* createWindow(bool autoCreateWindow, GLRenderSystem *system, const std::string &windowTitle);
	BWRenderWindow* newWindow(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams /* = 0 */);
	void addConfig();
	void refreshConfig();
	void stop();
	virtual void* getProcAddress(const std::string &procname);
	virtual std::string validateConfig();
private:
	typedef std::vector <DisplayMonitorInfo> DisplayMonitorInfoList;
	DisplayMonitorInfoList mMonitorInfoList;
	std::vector<DEVMODE> mDevModes;
	WIn32Window *mIntialWindow;
	std::vector<int> mFSAALevels;
	bool mHasPiexlFormatARB;
	bool mHasMultisample;
	bool mHasHardwareGamma;
};


#endif