#include "GLWin32Support.h"
#include <windef.h>
#include <WinGDI.h>
#include <WinUser.h>
#include "../BWEngineCore/BWStringConverter.h"
#include "../BWEngineCore/BWStringOperation.h"
#include "Win32Window.h"
#include "GLRenderSystem.h"
#include"GL/include/glew.h"


#if defined(_WIN32)
#include "GL/include/wglew.h"
#elif !idefined(__APPLE__) || defined(GLEW_APPLE_GLX)
#include <GL/include/glxew.h> 
#endif
#include "GL/include/wglext.h"
#ifndef GLEW_MX
#else

#endif // !GLEW_MX


GLWin32Support::GLWin32Support() :mIntialWindow(0)
, mHasPiexlFormatARB(false)
, mHasMultisample(false)
, mHasHardwareGamma(false)
{
	initialiseWGL();
}

GLWin32Support::~GLWin32Support()
{
}
template<class C> void remove_duplicates(C& c)
{
	std::sort(c.begin(), c.end());
	typename C::iterator p = std::unique(c.begin(), c.end());
	c.erase(p, c.end());
}

void GLWin32Support::start()
{
	//log 一下
}

void GLWin32Support::addConfig()
{
	//TODO: EnumDisplayDevices http://msdn.microsoft.com/library/en-us/gdi/devcons_2303.asp
	/*vector<string> DisplayDevices;
	DISPLAY_DEVICE DisplayDevice;
	DisplayDevice.cb = sizeof(DISPLAY_DEVICE);
	DWORD i=0;
	while (EnumDisplayDevices(NULL, i++, &DisplayDevice, 0) {
	DisplayDevices.push_back(DisplayDevice.DeviceName);
	}*/

	ConfigOption optFullScreen;
	ConfigOption optVideoMode;
	ConfigOption optColourDepth;
	ConfigOption optDisplayFrequency;
	ConfigOption optVSync;
	ConfigOption optVSyncInterval;
	ConfigOption optFSAA;
	ConfigOption optRTTMode;
	ConfigOption optSRGB;

	// FS setting possiblities
	optFullScreen.name = "Full Screen";
	optFullScreen.possibleValues.push_back("Yes");
	optFullScreen.possibleValues.push_back("No");
	optFullScreen.currentValue = "No";
	optFullScreen.immutable = false;

	// Video mode possiblities
	DEVMODE DevMode;
	DevMode.dmSize = sizeof(DEVMODE);
	optVideoMode.name = "Video Mode";
	optVideoMode.immutable = false;
	for (DWORD i = 0; EnumDisplaySettings(NULL, i, &DevMode); ++i)
	{
		if (DevMode.dmBitsPerPel < 16 || DevMode.dmPelsHeight < 480)
			continue;
		mDevModes.push_back(DevMode);
		StringStream str;
		str << DevMode.dmPelsWidth << " x " << DevMode.dmPelsHeight;
		optVideoMode.possibleValues.push_back(str.str());
	}
	remove_duplicates(optVideoMode.possibleValues);
	optVideoMode.currentValue = optVideoMode.possibleValues.front();

	optColourDepth.name = "Colour Depth";
	optColourDepth.immutable = false;
	optColourDepth.currentValue.clear();

	optDisplayFrequency.name = "Display Frequency";
	optDisplayFrequency.immutable = false;
	optDisplayFrequency.currentValue.clear();

	optVSync.name = "VSync";
	optVSync.immutable = false;
	optVSync.possibleValues.push_back("No");
	optVSync.possibleValues.push_back("Yes");
	optVSync.currentValue = "No";

	optVSyncInterval.name = "VSync Interval";
	optVSyncInterval.immutable = false;
	optVSyncInterval.possibleValues.push_back("1");
	optVSyncInterval.possibleValues.push_back("2");
	optVSyncInterval.possibleValues.push_back("3");
	optVSyncInterval.possibleValues.push_back("4");
	optVSyncInterval.currentValue = "1";

	optFSAA.name = "FSAA";
	optFSAA.immutable = false;
	optFSAA.possibleValues.push_back("0");
	for (std::vector<int>::iterator it = mFSAALevels.begin(); it != mFSAALevels.end(); ++it)
	{
		std::string val = StringConverter::ToString(*it);
		optFSAA.possibleValues.push_back(val);
		/* not implementing CSAA in GL for now
		if (*it >= 8)
		optFSAA.possibleValues.push_back(val + " [Quality]");
		*/

	}
	optFSAA.currentValue = "0";

	optRTTMode.name = "RTT Preferred Mode";
	optRTTMode.possibleValues.push_back("FBO");
	optRTTMode.possibleValues.push_back("PBuffer");
	optRTTMode.possibleValues.push_back("Copy");
	optRTTMode.currentValue = "FBO";
	optRTTMode.immutable = false;


	// SRGB on auto window
	optSRGB.name = "sRGB Gamma Conversion";
	optSRGB.possibleValues.push_back("Yes");
	optSRGB.possibleValues.push_back("No");
	optSRGB.currentValue = "No";
	optSRGB.immutable = false;


	mOptions[optFullScreen.name] = optFullScreen;
	mOptions[optVideoMode.name] = optVideoMode;
	mOptions[optColourDepth.name] = optColourDepth;
	mOptions[optDisplayFrequency.name] = optDisplayFrequency;
	mOptions[optVSync.name] = optVSync;
	mOptions[optVSyncInterval.name] = optVSyncInterval;
	mOptions[optFSAA.name] = optFSAA;
	mOptions[optRTTMode.name] = optRTTMode;
	mOptions[optSRGB.name] = optSRGB;

	refreshConfig();
}
BWRenderWindow* GLWin32Support::newWindow(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams)
{
	WIn32Window* window = new WIn32Window(*this);
	NameValuePairList newParams;

	if (miscParams != NULL)
	{
		newParams = *miscParams;
		miscParams = &newParams;

		NameValuePairList::const_iterator monitorIndexIt = miscParams->find("monitorIndex");
		HMONITOR hMonitor = NULL;
		int monitorIndex = -1;

		// If monitor index found, try to assign the monitor handle based on it.
		if (monitorIndexIt != miscParams->end())
		{
			if (mMonitorInfoList.empty())
				EnumDisplayMonitors(NULL, NULL, sCreateMonitorsInfoEnumProc, (LPARAM)&mMonitorInfoList);

			monitorIndex = StringConverter::ParseInt(monitorIndexIt->second);
			if (monitorIndex < (int)mMonitorInfoList.size())
			{
				hMonitor = mMonitorInfoList[monitorIndex].hMonitor;
			}
		}
		// If we didn't specified the monitor index, or if it didn't find it
		if (hMonitor == NULL)
		{
			POINT windowAnchorPoint;

			NameValuePairList::const_iterator opt;
			int left = -1;
			int top = -1;

			if ((opt = newParams.find("left")) != newParams.end())
				left = StringConverter::ParseInt(opt->second);

			if ((opt = newParams.find("top")) != newParams.end())
				top = StringConverter::ParseInt(opt->second);

			// Fill in anchor point.
			windowAnchorPoint.x = left;
			windowAnchorPoint.y = top;


			// Get the nearest monitor to this window.
			hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTONEAREST);
		}

		newParams["monitorHandle"] = StringConverter::ToString((size_t)hMonitor);
	}

	window->create(name, width, height, fullScreen, miscParams);

	if (!mIntialWindow)
		mIntialWindow = window;
	return window;
}
BWRenderWindow* GLWin32Support::createWindow(bool autoCreateWindow, GLRenderSystem *system, const std::string &windowTitle)
{
	if (autoCreateWindow)
	{
		ConfigOptionMap::iterator opt = mOptions.find("Full Screen");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find full screen options!", "Win32GLSupport::createWindow");
		bool fullscreen = (opt->second.currentValue == "Yes");

		opt = mOptions.find("Video Mode");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find video mode options!", "Win32GLSupport::createWindow");
		std::string val = opt->second.currentValue;
		std::string::size_type pos = val.find('x');
		if (pos == std::string::npos)
			assert(0);	//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Invalid Video Mode provided", "Win32GLSupport::createWindow");

		unsigned int w = StringConverter::ParseUnsignedInt(val.substr(0, pos));
		unsigned int h = StringConverter::ParseUnsignedInt(val.substr(pos + 1));

		// Parse optional parameters
		NameValuePairList winOptions;
		opt = mOptions.find("Colour Depth");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find Colour Depth options!", "Win32GLSupport::createWindow");
		unsigned int colourDepth =
			StringConverter::ParseUnsignedInt(opt->second.currentValue);
		winOptions["colourDepth"] = StringConverter::ToString(colourDepth);

		opt = mOptions.find("VSync");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find VSync options!", "Win32GLSupport::createWindow");
		bool vsync = (opt->second.currentValue == "Yes");
		winOptions["vsync"] = StringConverter::ToString(vsync);
		system->setWaitForVerticalBlank(vsync);

		opt = mOptions.find("VSync Interval");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find VSync Interval options!", "Win32GLSupport::createWindow");
		winOptions["vsyncInterval"] = opt->second.currentValue;


		opt = mOptions.find("Display Frequency");
		if (opt != mOptions.end())
		{
			unsigned int displayFrequency =
				StringConverter::ParseUnsignedInt(opt->second.currentValue);
			winOptions["displayFrequency"] = StringConverter::ToString(displayFrequency);
		}

		opt = mOptions.find("FSAA");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find FSAA options!", "Win32GLSupport::createWindow");
		StringVector aavalues = StringUtil::Split(opt->second.currentValue, " ", 1);
		unsigned int multisample = StringConverter::ParseUnsignedInt(aavalues[0]);
		std::string multisample_hint;
		if (aavalues.size() > 1)
			multisample_hint = aavalues[1];

		winOptions["FSAA"] = StringConverter::ToString(multisample);
		winOptions["FSAAHint"] = multisample_hint;

		opt = mOptions.find("sRGB Gamma Conversion");
		if (opt == mOptions.end())
			assert(0);//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find sRGB options!", "Win32GLSupport::createWindow");
		bool hwGamma = (opt->second.currentValue == "Yes");
		winOptions["gamma"] = StringConverter::ToString(hwGamma);

		return system->_createRenderWindow(windowTitle, w, h, fullscreen, &winOptions);
	}
	else
	{
		// XXX What is the else?
		return NULL;
	}
}
std::string GLWin32Support::validateConfig()
{
	return StringUtil::BLANK;
}
void GLWin32Support::stop()
{
	mIntialWindow = NULL;
}
void* GLWin32Support::getProcAddress(const std::string &procname)
{
	return (void *) wglGetProcAddress(procname.c_str());
}

void GLWin32Support::refreshConfig()
{
	ConfigOptionMap::iterator optVideoMode = mOptions.find("Video Mode");
	ConfigOptionMap::iterator moptColourDepth = mOptions.find("Colour Depth");
	ConfigOptionMap::iterator moptDisplayFrequency = mOptions.find("Display Frequency");
	if (optVideoMode == mOptions.end() || moptColourDepth == mOptions.end() || moptDisplayFrequency == mOptions.end())
		assert(0);	//OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Can't find mOptions!", "Win32GLSupport::refreshConfig");
	ConfigOption* optColourDepth = &moptColourDepth->second;
	ConfigOption* optDisplayFrequency = &moptDisplayFrequency->second;

	const std::string& val = optVideoMode->second.currentValue;
	std::string::size_type pos = val.find('x');
	if (pos == std::string::npos)
		assert(0);// (Exception::ERR_INVALIDPARAMS, "Invalid Video Mode provided", "Win32GLSupport::refreshConfig");
	DWORD width = StringConverter::ParseUnsignedInt(val.substr(0, pos));
	DWORD height = StringConverter::ParseUnsignedInt(val.substr(pos + 1, std::string::npos));

	for (std::vector<DEVMODE>::const_iterator i = mDevModes.begin(); i != mDevModes.end(); ++i)
	{
		if (i->dmPelsWidth != width || i->dmPelsHeight != height)
			continue;
		optColourDepth->possibleValues.push_back(StringConverter::ToString((unsigned int)i->dmBitsPerPel));
		optDisplayFrequency->possibleValues.push_back(StringConverter::ToString((unsigned int)i->dmDisplayFrequency));
	}
	remove_duplicates(optColourDepth->possibleValues);
	remove_duplicates(optDisplayFrequency->possibleValues);
	optColourDepth->currentValue = optColourDepth->possibleValues.back();
	bool freqValid = std::find(optDisplayFrequency->possibleValues.begin(),
		optDisplayFrequency->possibleValues.end(),
		optDisplayFrequency->currentValue) != optDisplayFrequency->possibleValues.end();

	if ((optDisplayFrequency->currentValue != "N/A") && !freqValid)
		optDisplayFrequency->currentValue = optDisplayFrequency->possibleValues.front();
}



LRESULT CALLBACK GLWin32Support::dummyWndProc(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp)
{
	return DefWindowProc(hwnd, umsg, wp, lp);
}
BOOL  CALLBACK GLWin32Support::sCreateMonitorsInfoEnumProc(HMONITOR Hmonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	DisplayMonitorInfoList *pArrmonitorsInfor = (DisplayMonitorInfoList*)dwData;
	DisplayMonitorInfo displayMonitorInfo;
	displayMonitorInfo.hMonitor = Hmonitor;
	memset(&displayMonitorInfo.monitorInfoEX, 0, sizeof(MONITORINFOEX));
	displayMonitorInfo.monitorInfoEX.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfoA(Hmonitor, &displayMonitorInfo.monitorInfoEX);
	pArrmonitorsInfor->push_back(displayMonitorInfo);
	return TRUE;
}
	//只是测试一下 并没有创建真正的窗口
void GLWin32Support::initialiseWGL()
{
	LPCSTR dummyText = "OgreWglDummy";
#ifdef OGRE_STATIC_LIB
	HINSTANCE hinst = GetModuleHandle(NULL);
#else
#  if OGRE_DEBUG_MODE == 1
	HINSTANCE hinst = GetModuleHandle("RenderSystem_GL_d.dll");
#  else
	HINSTANCE hinst = GetModuleHandle("RenderSystem_GL.dll");
#  endif
#endif

	WNDCLASS dummyClass;
	memset(&dummyClass, 0, sizeof(WNDCLASS));
	dummyClass.style = CS_OWNDC;
	dummyClass.hInstance = hinst;
	dummyClass.lpfnWndProc = dummyWndProc;
	dummyClass.lpszClassName = dummyText;
	RegisterClass(&dummyClass);

	HWND hwnd = CreateWindow(dummyText, dummyText,
		WS_POPUP | WS_CLIPCHILDREN,
		0, 0, 32, 32, 0, 0, hinst, 0);

	// if a simple CreateWindow fails, then boy are we in trouble...
	if (hwnd == NULL)
		assert(0);//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "CreateWindow() failed", "Win32GLSupport::initializeWGL");


	// no chance of failure and no need to release thanks to CS_OWNDC
	HDC hdc = GetDC(hwnd);

	// assign a simple OpenGL pixel format that everyone supports
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.cColorBits = 16;
	pfd.cDepthBits = 15;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;

	// if these fail, wglCreateContext will also quietly fail
	int format;
	if ((format = ChoosePixelFormat(hdc, &pfd)) != 0)
		SetPixelFormat(hdc, format, &pfd);

	HGLRC hrc = wglCreateContext(hdc);
	if (hrc)
	{
		HGLRC oldrc = wglGetCurrentContext();
		HDC oldhdc = wglGetCurrentDC();
		// if wglMakeCurrent fails, wglGetProcAddress will return null
		wglMakeCurrent(hdc, hrc);

		PFNWGLGETEXTENSIONSSTRINGARBPROC _wglGetExtensionsStringARB =
			(PFNWGLGETEXTENSIONSSTRINGARBPROC)
			wglGetProcAddress("wglGetExtensionsStringARB");

		// check for pixel format and multisampling support
		if (_wglGetExtensionsStringARB)
		{
			std::istringstream wglexts(_wglGetExtensionsStringARB(hdc));
			std::string ext;
			while (wglexts >> ext)
			{
				if (ext == "WGL_ARB_pixel_format")
					mHasPiexlFormatARB = true;
				else if (ext == "WGL_ARB_multisample")
					mHasMultisample = true;
				else if (ext == "WGL_EXT_framebuffer_sRGB")
					mHasHardwareGamma = true;
			}
		}

		if (mHasPiexlFormatARB && mHasMultisample)
		{
			// enumerate all formats w/ multisampling
			static const int iattr[] = {
				WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
				WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
				WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
				WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
				/* We are no matter about the colour, depth and stencil buffers here
				WGL_COLOR_BITS_ARB, 24,
				WGL_ALPHA_BITS_ARB, 8,
				WGL_DEPTH_BITS_ARB, 24,
				WGL_STENCIL_BITS_ARB, 8,
				*/
				WGL_SAMPLES_ARB, 2,
				0
			};
			int formats[256];
			unsigned int count;
			// cheating here.  wglChoosePixelFormatARB procc address needed later on
			// when a valid GL context does not exist and glew is not initialized yet.
			WGLEW_GET_FUN(__wglewChoosePixelFormatARB) =
				(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");
			if (WGLEW_GET_FUN(__wglewChoosePixelFormatARB)(hdc, iattr, 0, 256, formats, &count))
			{
				// determine what multisampling levels are offered
				int query = WGL_SAMPLES_ARB, samples;
				for (unsigned int i = 0; i < count; ++i)
				{
					PFNWGLGETPIXELFORMATATTRIBIVARBPROC _wglGetPixelFormatAttribivARB =
						(PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
						wglGetProcAddress("wglGetPixelFormatAttribivARB");
					if (_wglGetPixelFormatAttribivARB(hdc, formats[i], 0, 1, &query, &samples))
					{
						mFSAALevels.push_back(samples);
					}
				}
				remove_duplicates(mFSAALevels);
			}
		}

		wglMakeCurrent(oldhdc, oldrc);
		wglDeleteContext(hrc);
	}

	// clean up our dummy window and class
	DestroyWindow(hwnd);
	UnregisterClass(dummyText, hinst);
}

void GLWin32Support::initialiseExtension()
{
	assert(mIntialWindow);
	GLSupport::initialiseExtension();
	glewInit();
	//wglewContextInit(this);
	
	PFNWGLGETEXTENSIONSSTRINGARBPROC _wgelGEtExtensionsString =
		(PFNWGLGETEXTENSIONSSTRINGARBPROC)(wglGetProcAddress("wglGetExtensionStringARB"));
	if (!_wgelGEtExtensionsString)
	{
		return;
	}
	const char *wgl_extension = _wgelGEtExtensionsString(mIntialWindow->getHDC());
	//log
	StringStream ext;
	std::string instr;
	ext << wgl_extension;
	while (ext >> instr)
	{
		extensionList.insert(instr);
	}
}
bool GLWin32Support::selectPixelFormat(HDC hdc, int colourDepht, int multiSample, bool hwGamma)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = (colourDepht > 16) ? 24 : colourDepht;
	pfd.cAlphaBits = (colourDepht > 16) ? 8 : 0;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;

	int format = 0;
	int useHwGamma = hwGamma ? GL_TRUE : GL_FALSE;

	if (multiSample && (!mHasMultisample || !mHasPiexlFormatARB))
	{
		return false;
	}
	if (hwGamma && !mHasHardwareGamma)
	{
		return false;
	}
	if ((multiSample || hwGamma) && WGLEW_GET_FUN(__wglewChoosePixelFormatARB))
	{
		// Use WGL to test extended caps (multisample, sRGB)
		std::vector<int> attribList;
		attribList.push_back(WGL_DRAW_TO_WINDOW_ARB); attribList.push_back(GL_TRUE);
		attribList.push_back(WGL_SUPPORT_OPENGL_ARB); attribList.push_back(GL_TRUE);
		attribList.push_back(WGL_DOUBLE_BUFFER_ARB); attribList.push_back(GL_TRUE);
		attribList.push_back(WGL_SAMPLE_BUFFERS_ARB); attribList.push_back(GL_TRUE);
		attribList.push_back(WGL_ACCELERATION_ARB); attribList.push_back(WGL_FULL_ACCELERATION_ARB);
		attribList.push_back(WGL_COLOR_BITS_ARB); attribList.push_back(pfd.cColorBits);
		attribList.push_back(WGL_ALPHA_BITS_ARB); attribList.push_back(pfd.cAlphaBits);
		attribList.push_back(WGL_DEPTH_BITS_ARB); attribList.push_back(24);
		attribList.push_back(WGL_STENCIL_BITS_ARB); attribList.push_back(8);
		attribList.push_back(WGL_SAMPLES_ARB); attribList.push_back(multiSample);
		if (useHwGamma && checkExtension("WGL_EXT_framebuffer_sRGB"))
		{
			attribList.push_back(WGL_FRAMEBUFFER_SRGB_CAPABLE_EXT); attribList.push_back(GL_TRUE);
		}
		// terminator
		attribList.push_back(0);


		UINT nformats;
		// ChoosePixelFormatARB proc address was obtained when setting up a dummy GL context in initialiseWGL()
		// since glew hasn't been initialized yet, we have to cheat and use the previously obtained address
		if (!WGLEW_GET_FUN(__wglewChoosePixelFormatARB)(hdc, &(attribList[0]), NULL, 1, &format, &nformats) || nformats <= 0)
			return false;

	}
	else
	{
		format = ChoosePixelFormat(hdc, &pfd);
	}


	return (format && SetPixelFormat(hdc, format, &pfd));
}