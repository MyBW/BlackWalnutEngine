#include "Win32Window.h"
#include <windef.h>
#include <WinGDI.h>
#include <WinUser.h>
#include <assert.h>
#include<iostream>
#include "GL/include/wglew.h"
#include "../BWEngineCore/BWStringConverter.h"
#include "../BWEngineCore/BWRoot.h"
#include "../BWEngineCore/BWViewport.h"
#include "../BWEngineCore/OgreWindowEventUtilities.h"
#include"../BWEngineCore/BWMaterialManager.h"
#include"../BWEngineCore/BWSceneManager.h"
#include"../BWEngineCore/BWAutoParamDataSource.h"
#include "GLPixelUtil.h"
#include "GLPreDefine.h"
#include "GLSLGpuProgram.h"
#include "GLRenderSystem.h"
#include"GLTexture.h"
#include"GLHardwareVertexBuffer.h"
#include"GLHardwareIndexBuffer.h"
#include"GLRenderBuffer.h"
#include"GLRenderTarget.h"
#include "GL/include/glut.h"
#define  _MAX_CLASS_NAME_ 128
WIn32Window::WIn32Window(GLWin32Support &glsupport) :
mSupport(&glsupport),
mContext(0)
{
	mFullScreen = false;
	mHWnd = 0;
	mGlrc = 0;
	mIsExternal = false;
	mIsExternalGLControl = false;
	mIsExternalGLContext = false;
	mSizing = false;
	mClosed = false;
	mDisplayFrequency = 0;
	mActive = false;
	mDeviceName = NULL;
}

WIn32Window::~WIn32Window()
{
}
void WIn32Window::_setRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WIn32Window::_endUpdate()
{
	SwapBuffers(mHdc);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void WIn32Window::clearRenderTarget()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//////////////////////////////////////////////////生成



bool WIn32Window::InitWindow()
{
	return true;
	material = BWMaterialManager::GetInstance()->GetResource("DirectLightDefferRendering", "General");
	if (material.IsNull())
	{
		Log::GetInstance()->logMessage("WIn32Window::InitWindow()  cant get the DefferRendering material");
		return false;
	}
	BWPass* pass = material->getTechnique(0)->GetPass(0);
	gpuPrgramUsage = pass->getGPUProgramUsage();

	glslProgram = dynamic_cast<GLSLGpuProgram*>(gpuPrgramUsage->GetGpuProgram().Get());
	BWGpuProgramParametersPtr parameter = pass->getGPUProgramParameter();
	if (parameter.Get() != NULL)
	{
		const BWGpuProgramParameters::AutoConstantList &autoConstantList = parameter->GetAutoConstantList();
		BWGpuProgramParameters::AutoConstantList::const_iterator itor = autoConstantList.begin();
		while (itor != autoConstantList.end())
		{
			switch (itor->paramType)
			{
			case BWGpuProgramParameters::ACT_VIEW_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY;
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_WORLD_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY; // 这个表示的是 Model Matirx
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_PROJECTION_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY;
				tmp.M00 = 0.750034690;
				tmp.M11 = 1.00004637;
				tmp.M22 = -1.00002003;
				tmp.M23 = -0.200002003;
				tmp.M32 = -1.0f;
				tmp.M33 = 0.0f;
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			default:
				break;
			}
			itor++;
		}
	}
	glslProgram->bind();
	GpuNamedConstantsPtr namedConstant = gpuPrgramUsage->GetGpuProgramParameter()->getNamedConstants();
	if (namedConstant.IsNull())
	{
		namedConstant = glslProgram->GetDefaultParameters()->getNamedConstants();
	}
	//设置各种unifomr参数
	GpuConstantDefinitionMap::iterator itor = namedConstant->map.begin();
	while (itor != namedConstant->map.end())
	{
		GpuConstantDefinition *def = &(itor->second);
		int size = def->arraySize * itor->second.getElementSize(def->constType, false);
		void *data = NULL;
		int a = def->physicalIndex;
		if (def->isFloat())
		{
			data = gpuPrgramUsage->GetGpuProgramParameter()->GetFloatPointer(def->physicalIndex);
		}
		else
		{
			data = gpuPrgramUsage->GetGpuProgramParameter()->GetIntPointer(def->physicalIndex);
		}
		//if (itor->second.getElementSize(def->constType, false) > 4)
		//{
		//	// 说明是矩阵需要转置
		//	GLRenderSystem::GLtranspose(def->constType, data);
		//}
		glslProgram->SetParameter(itor->first, data , def->constType);
		itor++;
	}
	CHECK_GL_ERROR(glGenBuffers(1, &BufferID));
	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, BufferID));
	GLfloat Z = 3.0 / 4;
	GLfloat QUADS[] = {
		-1.0f , 0.75f ,  -Z , 0.0f , 1.0f,
		-1.0f , -0.75f , -Z , 0.0f , 0.0f,
		1.0f , -0.75f ,  -Z, 1.0f , 0.0f,
		-1.0f , 0.75f ,  -Z , 0.0f , 1.0f,
		1.0f , -0.75f ,  -Z , 1.0f , 0.0f,
		1.0f , 0.75f ,   -Z, 1.0f , 1.0f };
	glBufferData(GL_ARRAY_BUFFER, sizeof(QUADS), QUADS, GL_STATIC_DRAW);
	BaseColorMapLoc = glGetUniformLocation(glslProgram->getID(), "BaseColorMap");
	NormalMapLoc = glGetUniformLocation(glslProgram->getID(), "NormalMap");
	PositionMapLoc = glGetUniformLocation(glslProgram->getID(), "PositionMap");


	GBuffer = BWRoot::GetInstance()->getRenderSystem()->getGBuffer();
	if (GBuffer == NULL)
	{
		Log::GetInstance()->logMessage("WIn32Window::InitWindow()  Cant Get RenderBuffer ");
		return false;
	}
	ABufferTexture = GBuffer->getTextureBuffer("BaseColorMap");
	BBufferTexture = GBuffer->getTextureBuffer("NormalMap");
	CBufferTexture = GBuffer->getTextureBuffer("PositionMap");

	//这里生成的点光源的延迟光照
	mPointLightDefferLightingMaterial = BWMaterialManager::GetInstance()->GetResource("PointLightDefferLightting", "General");
	if (mPointLightDefferLightingMaterial.IsNull())
	{
		Log::GetInstance()->logMessage("WIn32Window::InitWindow()  cant get the PointLightDefferLightting material");
		return false;
	}
	pass = mPointLightDefferLightingMaterial->getTechnique(0)->GetPass(0);
	mPointLightDefferLightingGpuPrgramUsage = pass->getGPUProgramUsage();

	mPointLightDefferLightingGLSLProgram = dynamic_cast<GLSLGpuProgram*>(mPointLightDefferLightingGpuPrgramUsage->GetGpuProgram().Get());
	parameter = pass->getGPUProgramParameter();
	if (parameter.Get() != NULL)
	{
		const BWGpuProgramParameters::AutoConstantList &autoConstantList = parameter->GetAutoConstantList();
		BWGpuProgramParameters::AutoConstantList::const_iterator itor = autoConstantList.begin();
		while (itor != autoConstantList.end())
		{
			switch (itor->paramType)
			{
			case BWGpuProgramParameters::ACT_VIEW_MATRIX:
			{
				//这里有些问题 因为渲染流程的起始点是renderTarget到camera  而在windrendertarget中是没有camera的 
				//这里暂时让wind使用的摄像机和上一次的摄像机保持一致
				//BWMatrix4 tmp = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->getViewMatrix();
				BWMatrix4 tmp;
				viewportPriorityList[0]->getCamare()->getWorldTransforms(&tmp);
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_WORLD_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY; 
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_PROJECTION_MATRIX:
			{
				
				//BWMatrix4 tmp = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->getProjectionMatrix();
				BWMatrix4 tmp = viewportPriorityList[0]->getCamare()->getProjectionMatrixRS();
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			default:
				break;
			}
			itor++;
		}
	}
	mPointLightDefferLightingGLSLProgram->bind();
	namedConstant = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->getNamedConstants();
	if (namedConstant.IsNull())
	{
		namedConstant = mPointLightDefferLightingGLSLProgram->GetDefaultParameters()->getNamedConstants();
	}
	//设置各种自动unifomr参数
	 itor = namedConstant->map.begin();
	while (itor != namedConstant->map.end())
	{
		GpuConstantDefinition *def = &(itor->second);
		int size = def->arraySize * itor->second.getElementSize(def->constType, false);
		void *data = NULL;
		int a = def->physicalIndex;
		if (def->isFloat())
		{
			data = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->GetFloatPointer(def->physicalIndex);
		}
		else
		{
			data = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->GetIntPointer(def->physicalIndex);
		}
		//if (itor->second.getElementSize(def->constType, false) > 4)
		//{
		//	// 说明是矩阵需要转置
		//	GLRenderSystem::GLtranspose(def->constType, data);
		//}
		mPointLightDefferLightingGLSLProgram->SetParameter(itor->first, data, def->constType);
		itor++;
	}

	BWMeshPrt  mPointMesh = BWRoot::GetInstance()->getRenderSystem()->getPointLightMesh();
	BWSubMesh * mPointSubMesh = mPointMesh->getSubMesh(0);
	VertexDataPrt mVertextData = mPointSubMesh->getVertexData();
	IndexDataPrt mIndexData = mPointSubMesh->getIndexData();
	GLHardwareVertexBuffer* vertexBuffer = dynamic_cast<GLHardwareVertexBuffer*>(mVertextData->mVertexBufferBind->getBuffer(0).Get());
	GLHardwareIndexBuffer* indexBuffer = dynamic_cast<GLHardwareIndexBuffer*>(mIndexData->mIndexBuffer.Get());
	
	mPointLightDefferLightingBufferID = vertexBuffer->getGLHardwareBufferID();
	mPointLightDefferLightingIndexBufferID = indexBuffer->getGLBufferID();
	SphereVertexNum = 3840; // index number~
	/*mPointLightDefferLightingBaseColorMapLoc = glGetUniformLocation(mPointLightDefferLightingGLSLProgram->getID(), "BaseColorMap");
	mPointLightDefferLightingNormalMapLoc = glGetUniformLocation(mPointLightDefferLightingGLSLProgram->getID(), "NormalMap");
	mPointLightDefferLightingPositionMapLoc = glGetUniformLocation(mPointLightDefferLightingGLSLProgram->getID(), "PositionMap");*/
}

void WIn32Window::rePosition(int left, int top)
{
	if (mHWnd && !mFullScreen)
	{
		SetWindowPos(mHWnd, 0, left, top, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
void WIn32Window::create(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams /* = 0 */)
{
	// destroy current window, if any
	if (mHWnd)
		destroy();

#ifdef OGRE_STATIC_LIB
	HINSTANCE hInst = GetModuleHandle(NULL);
#else
#  if OGRE_DEBUG_MODE == 1
	HINSTANCE hInst = GetModuleHandle("RenderSystem_GL_d.dll");
#  else
	HINSTANCE hInst = GetModuleHandle("RenderSystem_GL.dll");
	//hInst = GetModuleHandle(NULL);
#  endif
#endif

	mHWnd = 0;
	mName = name;
	mFullScreen = fullScreen;
	mClosed = false;
	mDisplayFrequency = 0;
	mIsDepthBuffered = true;
	mColourDepth = mFullScreen ? 32 : GetDeviceCaps(GetDC(0), BITSPIXEL);
	int left = -1; // Defaults to screen center
	int top = -1; // Defaults to screen center
	HWND parent = 0;
	std::string title = name;
	bool vsync = false;
	unsigned int vsyncInterval = 1;
	std::string border;
	bool outerSize = false;
	bool hwGamma = false;
	int monitorIndex = -1;
	HMONITOR hMonitor = NULL;

	if (miscParams)
	{
		// Get variable-length params
		NameValuePairList::const_iterator opt;
		NameValuePairList::const_iterator end = miscParams->end();

		if ((opt = miscParams->find("title")) != end)
			title = opt->second;

		if ((opt = miscParams->find("left")) != end)
			left = StringConverter::ParseInt(opt->second);

		if ((opt = miscParams->find("top")) != end)
			top = StringConverter::ParseInt(opt->second);

		if ((opt = miscParams->find("depthBuffer")) != end)
			mIsDepthBuffered = StringConverter::ParseBool(opt->second);

		if ((opt = miscParams->find("vsync")) != end)
			vsync = StringConverter::ParseBool(opt->second);

		if ((opt = miscParams->find("vsyncInterval")) != end)
			vsyncInterval = StringConverter::ParseUnsignedInt(opt->second);

		if ((opt = miscParams->find("FSAA")) != end)
			mFSAA = StringConverter::ParseUnsignedInt(opt->second);

		if ((opt = miscParams->find("FSAAHint")) != end)
			mFSAAHint = opt->second;

		if ((opt = miscParams->find("gamma")) != end)
			hwGamma = StringConverter::ParseBool(opt->second);

		if ((opt = miscParams->find("externalWindowHandle")) != end)
		{
			mHWnd = (HWND)StringConverter::ParseUnsignedInt(opt->second);
			if (mHWnd)
			{
				mIsExternal = true;
				mFullScreen = false;
			}

			if ((opt = miscParams->find("externalGLControl")) != end) {
				mIsExternalGLControl = StringConverter::ParseBool(opt->second);
			}
		}
		if ((opt = miscParams->find("externalGLContext")) != end)
		{
			mGlrc = (HGLRC)StringConverter::ParseUnsignedLong(opt->second);
			if (mGlrc)
				mIsExternalGLContext = true;
		}

		// window border style
		opt = miscParams->find("border");
		if (opt != miscParams->end())
			border = opt->second;
		// set outer dimensions?
		opt = miscParams->find("outerDimensions");
		if (opt != miscParams->end())
			outerSize = StringConverter::ParseBool(opt->second);

		// only available with fullscreen
		if ((opt = miscParams->find("displayFrequency")) != end)
			mDisplayFrequency = StringConverter::ParseUnsignedInt(opt->second);
		if ((opt = miscParams->find("colourDepth")) != end)
		{
			mColourDepth = StringConverter::ParseUnsignedInt(opt->second);
			if (!mFullScreen)
			{
				// make sure we don't exceed desktop colour depth
				if ((int)mColourDepth > GetDeviceCaps(GetDC(0), BITSPIXEL))
					mColourDepth = GetDeviceCaps(GetDC(0), BITSPIXEL);
			}
		}

		// incompatible with fullscreen
		if ((opt = miscParams->find("parentWindowHandle")) != end)
			parent = (HWND)StringConverter::ParseUnsignedInt(opt->second);


		// monitor index
		if ((opt = miscParams->find("monitorIndex")) != end)
			monitorIndex = StringConverter::ParseInt(opt->second);

		// monitor handle
		if ((opt = miscParams->find("monitorHandle")) != end)
			hMonitor = (HMONITOR)StringConverter::ParseInt(opt->second);
	}

	if (!mIsExternal)
	{
		DWORD		  dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		DWORD		  dwStyleEx = 0;
		MONITORINFOEX monitorInfoEx;
		RECT		  rc;

		// If we didn't specified the adapter index, or if it didn't find it
		if (hMonitor == NULL)
		{
			POINT windowAnchorPoint;

			// Fill in anchor point.
			windowAnchorPoint.x = left;
			windowAnchorPoint.y = top;


			// Get the nearest monitor to this window.
			hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTONEAREST);
		}

		// Get the target monitor info		
		memset(&monitorInfoEx, 0, sizeof(MONITORINFOEX));
		monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfoEx);

		size_t devNameLen = strlen(monitorInfoEx.szDevice);
		mDeviceName = new char[devNameLen + 1];

		strcpy_s(mDeviceName, 32 , monitorInfoEx.szDevice);


		// No specified top left -> Center the window in the middle of the monitor
		if (left == -1 || top == -1)
		{
			int screenw = monitorInfoEx.rcWork.right - monitorInfoEx.rcWork.left;
			int screenh = monitorInfoEx.rcWork.bottom - monitorInfoEx.rcWork.top;

			unsigned int winWidth, winHeight;
			adjustWindow(width, height, &winWidth, &winHeight);

			// clamp window dimensions to screen size
			int outerw = (winWidth < screenw) ? winWidth : screenw;
			int outerh = (winHeight < screenh) ? winHeight : screenh;

			if (left == -1)
				left = monitorInfoEx.rcWork.left + (screenw - outerw) / 2;
			else if (monitorIndex != -1)
				left += monitorInfoEx.rcWork.left;

			if (top == -1)
				top = monitorInfoEx.rcWork.top + (screenh - outerh) / 2;
			else if (monitorIndex != -1)
				top += monitorInfoEx.rcWork.top;
		}
		else if (monitorIndex != -1)
		{
			left += monitorInfoEx.rcWork.left;
			top += monitorInfoEx.rcWork.top;
		}

		mWidth = width;
		mHeight = height;
		mtop = top;
		mleft = left;

		if (mFullScreen)
		{
			dwStyle |= WS_POPUP;
			dwStyleEx |= WS_EX_TOPMOST;
			mtop = monitorInfoEx.rcMonitor.top;
			mleft = monitorInfoEx.rcMonitor.left;
		}
		else
		{
			if (parent)
			{
				dwStyle |= WS_CHILD;
			}
			else
			{
				if (border == "none")
					dwStyle |= WS_POPUP;
				else if (border == "fixed")
					dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
					WS_SYSMENU | WS_MINIMIZEBOX;
				else
					dwStyle |= WS_OVERLAPPEDWINDOW;
			}

			int screenw = GetSystemMetrics(SM_CXSCREEN);
			int screenh = GetSystemMetrics(SM_CYSCREEN);

			if (!outerSize)
			{
				// Calculate window dimensions required
				// to get the requested client area
				SetRect(&rc, 0, 0, mWidth, mHeight);
				AdjustWindowRect(&rc, dwStyle, false);
				mWidth = rc.right - rc.left;
				mHeight = rc.bottom - rc.top;

				// Clamp window rect to the nearest display monitor.
				if (mleft < monitorInfoEx.rcWork.left)
					mleft = monitorInfoEx.rcWork.left;

				if (mtop < monitorInfoEx.rcWork.top)
					mtop = monitorInfoEx.rcWork.top;

				if ((int)mWidth > monitorInfoEx.rcWork.right - mleft)
					mWidth = monitorInfoEx.rcWork.right - mleft;

				if ((int)mHeight > monitorInfoEx.rcWork.bottom - mtop)
					mHeight = monitorInfoEx.rcWork.bottom - mtop;
			}
		}

		// register class and create window

		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WindowEventUtilities::_WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInst;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = L"GLWindow";
		wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		RegisterClassExW(&wcex);


		/*WNDCLASS wc = { CS_OWNDC, WindowEventUtilities::_WndProc, 0, 0, hInst,
			LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, "GLWindow" };
		if (!RegisterClass(&wc))
		{
			assert(0);
		}*/

		if (mFullScreen)
		{
			DEVMODE displayDeviceMode;

			memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
			displayDeviceMode.dmSize = sizeof(DEVMODE);
			displayDeviceMode.dmBitsPerPel = mColourDepth;
			displayDeviceMode.dmPelsWidth = mWidth;
			displayDeviceMode.dmPelsHeight = mHeight;
			displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			if (mDisplayFrequency)
			{
				displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
				displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
				{
					/*LogManager::getSingleton().logMessage(LML_NORMAL, "ChangeDisplaySettings with user display frequency failed");
					displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;*/
				}
			}
			if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL);
				//LogManager::getSingleton().logMessage(LML_CRITICAL, "ChangeDisplaySettings failed");
		}
		// Pass pointer to self as WM_CREATE parameter

		//WCHAR szTitle[100];                  // 标题栏文本
		//WCHAR szWindowClass[100];            // 主窗口类名
		//LoadStringW(hInst,100, szTitle, 100);
		//LoadStringW(hInst, 1001, szWindowClass,100);

		mHWnd = CreateWindowW(L"GLWindow",L"BWWindow",
			dwStyle, mleft, mtop, mWidth, mHeight, parent, 0, hInst, this);


		//mHWnd = CreateWindowEx(dwStyleEx, "GLWindow", title.c_str(),
		//	dwStyle, mleft, mtop, mWidth, mHeight, parent, 0, hInst, this);
		WindowEventUtilities::_addRenderWindow(this);

		/*LogManager::getSingleton().stream()
			<< "Created Win32Window '"
			<< mName << "' : " << mWidth << "x" << mHeight
			<< ", " << mColourDepth << "bpp";*/

	}

	HDC old_hdc = wglGetCurrentDC();
	HGLRC old_context = wglGetCurrentContext();

	RECT rc;
	// top and left represent outer window position
	GetWindowRect(mHWnd, &rc);
	mtop = rc.top;
	mleft = rc.left;
	// width and height represent drawable area only
	GetClientRect(mHWnd, &rc);
	mWidth = rc.right;
	mHeight = rc.bottom;

	mHdc = GetDC(mHWnd);

	if (!mIsExternalGLControl)
	{
		int testFsaa = mFSAA;
		bool testHwGamma = hwGamma;
		bool formatOk = mSupport->selectPixelFormat(mHdc, mColourDepth, testFsaa, testHwGamma);
		if (!formatOk)
		{
			if (mFSAA > 0)
			{
				// try without FSAA
				testFsaa = 0;
				formatOk = mSupport->selectPixelFormat(mHdc, mColourDepth, testFsaa, testHwGamma);
			}

			if (!formatOk && hwGamma)
			{
				// try without sRGB
				testHwGamma = false;
				testFsaa = mFSAA;
				formatOk = mSupport->selectPixelFormat(mHdc, mColourDepth, testFsaa, testHwGamma);
			}

			if (!formatOk && hwGamma && (mFSAA > 0))
			{
				// try without both
				testHwGamma = false;
				testFsaa = 0;
				formatOk = mSupport->selectPixelFormat(mHdc, mColourDepth, testFsaa, testHwGamma);
			}

			if (!formatOk)
				assert(0);
				/*OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
				"selectPixelFormat failed", "Win32Window::create");*/

		}
		// record what gamma option we used in the end
		// this will control enabling of sRGB state flags when used
		mHwGamma = testHwGamma;
		mFSAA = testFsaa;
	}
	if (!mIsExternalGLContext)
	{
		mGlrc = wglCreateContext(mHdc);
		if (!mGlrc)
			assert(0);
		/*	OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR,
			"wglCreateContext failed: " + translateWGLError(), "Win32Window::create");*/
	}
	if (!wglMakeCurrent(mHdc, mGlrc))
		assert(0);//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "wglMakeCurrent", "Win32Window::create");

	// Do not change vsync if the external window has the OpenGL control
	if (!mIsExternalGLControl) {
		// Don't use wglew as if this is the first window, we won't have initialised yet
		PFNWGLSWAPINTERVALEXTPROC _wglSwapIntervalEXT =
			(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		if (_wglSwapIntervalEXT)
			_wglSwapIntervalEXT(vsync ? vsyncInterval : 0);
	}

	if (old_context && old_context != mGlrc)
	{
		// Restore old context
		if (!wglMakeCurrent(old_hdc, old_context))
			assert(0);//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "wglMakeCurrent() failed", "Win32Window::create");

		// Share lists with old context
		if (!wglShareLists(old_context, mGlrc))
			assert(0);//OGRE_EXCEPT(Exception::ERR_RENDERINGAPI_ERROR, "wglShareLists() failed", " Win32Window::create");
	}
	// Create RenderSystem context
	mContext = new Win32Context(mHdc, mGlrc);

	mActive = true;
}
void WIn32Window::adjustWindow(unsigned int clientWidth, unsigned int clientHeight,
	unsigned int* winWidth, unsigned int* winHeight)
{
	// NB only call this for non full screen
	RECT rc;
	SetRect(&rc, 0, 0, clientWidth, clientHeight);
	AdjustWindowRect(&rc, WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, false);
	*winWidth = rc.right - rc.left;
	*winHeight = rc.bottom - rc.top;

	// adjust to monitor
	HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

	// Get monitor info	
	MONITORINFO monitorInfo;

	memset(&monitorInfo, 0, sizeof(MONITORINFO));
	monitorInfo.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMonitor, &monitorInfo);

	LONG maxW = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
	LONG maxH = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

	if (*winWidth > (unsigned int)maxW)
		*winWidth = maxW;
	if (*winHeight > (unsigned int)maxH)
		*winHeight = maxH;

}
void WIn32Window::setFullScreen(bool fullScreen, unsigned int width, unsigned int height)
{
	if (mFullScreen != fullScreen || width != mWidth || height != mHeight)
	{
		mFullScreen = fullScreen;
		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;

		if (mFullScreen)
		{
			dwStyle |= WS_POPUP;

			DEVMODE displayDeviceMode;

			memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
			displayDeviceMode.dmSize = sizeof(DEVMODE);
			displayDeviceMode.dmBitsPerPel = mColourDepth;
			displayDeviceMode.dmPelsWidth = width;
			displayDeviceMode.dmPelsHeight = height;
			displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			if (mDisplayFrequency)
			{
				displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
				displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL,
					CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
				{
					//LogManager::getSingleton().logMessage(LML_NORMAL, "ChangeDisplaySettings with user display frequency failed");
					displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
				}
			}
			else
			{
				// try a few
				displayDeviceMode.dmDisplayFrequency = 100;
				displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL,
					CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
				{
					displayDeviceMode.dmDisplayFrequency = 75;
					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL,
						CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
					{
						displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
					}
				}

			}
			// move window to 0,0 before display switch
			SetWindowPos(mHWnd, HWND_TOPMOST, 0, 0, mWidth, mHeight, SWP_NOACTIVATE);

			if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL);
				//LogManager::getSingleton().logMessage(LML_CRITICAL, "ChangeDisplaySettings failed");

			// Get the nearest monitor to this window.
			HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

			// Get monitor info	
			MONITORINFO monitorInfo;

			memset(&monitorInfo, 0, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &monitorInfo);

			mtop = monitorInfo.rcMonitor.top;
			mleft = monitorInfo.rcMonitor.left;

			SetWindowLong(mHWnd, GWL_STYLE, dwStyle);
			SetWindowPos(mHWnd, HWND_TOPMOST, mleft, mtop, width, height,
				SWP_NOACTIVATE);
			mWidth = width;
			mHeight = height;


		}
		else
		{
			dwStyle |= WS_OVERLAPPEDWINDOW;

			// drop out of fullscreen
			ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);

			// calculate overall dimensions for requested client area
			unsigned int winWidth, winHeight;
			adjustWindow(width, height, &winWidth, &winHeight);

			// deal with centreing when switching down to smaller resolution

			HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO monitorInfo;
			memset(&monitorInfo, 0, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &monitorInfo);

			LONG screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
			LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;


			int left = screenw > winWidth ? ((screenw - winWidth) / 2) : 0;
			int top = screenh > winHeight ? ((screenh - winHeight) / 2) : 0;

			SetWindowLong(mHWnd, GWL_STYLE, dwStyle);
			SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
				SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);
			mWidth = width;
			mHeight = height;

			windowMovedOrResized();

		}

	}
}
bool WIn32Window::isActive() const
{
	return mActive;
}
bool WIn32Window::isVisible() const
{
	return (mHWnd && IsIconic(mHWnd));
}
bool WIn32Window::isClosed() const
{
	return mClosed;
}
void WIn32Window::reposition(int left, int top)
{
	if (mHWnd && !mFullScreen)
	{
		SetWindowPos(mHWnd, 0, left, top, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
void WIn32Window::resize(unsigned int width, unsigned int height)
{
	if (mHWnd && !mFullScreen)
	{
		RECT rc = { 0, 0, width, height };
		AdjustWindowRect(&rc, GetWindowLong(mHWnd, GWL_STYLE), false);
		width = rc.right - rc.left;
		height = rc.bottom - rc.top;
		SetWindowPos(mHWnd, 0, 0, 0, width, height,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}
}
void WIn32Window::swapBuffers(bool waritForVSync /* = true */)
{
	if (!mIsExternalGLControl) {
		SwapBuffers(mHdc);
	}
}
void WIn32Window::copyContentsToMemory(const BWPixelBox &dst, FrameBuffer buffer)
{
	if ((dst.left < 0) || (dst.right > mWidth) ||
		(dst.top < 0) || (dst.bottom > mHeight) ||
		(dst.front != 0) || (dst.back != 1))
	{
		assert(0);
		/*OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"Invalid box.",
			"Win32Window::copyContentsToMemory");*/
	}

	if (buffer == FB_AUTO)
	{
		buffer = mFullScreen ? FB_FRONT : FB_BACK;
	}

	GLenum format = GLPixelUtil::getGLOriginFormat(dst.mPixelFormat);
	GLenum type = GLPixelUtil::getGLOriginDataType(dst.mPixelFormat);

	if ((format == GL_NONE) || (type == 0))
	{
		/*OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
			"Unsupported format.",
			"Win32Window::copyContentsToMemory");*/
		assert(0);
	}


	// Switch context if different from current one
	BWRenderSystem* rsys = BWRoot::GetInstance()->getRenderSystem();
	rsys->_setViewport(this->getViewport(0));

	// Must change the packing to ensure no overruns!
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadBuffer((buffer == FB_FRONT) ? GL_FRONT : GL_BACK);
	glReadPixels((GLint)dst.left, (GLint)dst.top,
		(GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
		format, type, dst.mData);

	// restore default alignment
	glPixelStorei(GL_PACK_ALIGNMENT, 4);

	//vertical flip
	{
		size_t rowSpan = dst.getWidth() * PixelUtil::getNumElemBytes(dst.mPixelFormat);
		size_t height = dst.getHeight();
		unsigned char *tmpData = new unsigned char[rowSpan * height];
		unsigned char *srcRow = (unsigned char*)dst.mData, *tmpRow = tmpData + (height - 1) * rowSpan;

		while (tmpRow >= tmpData)
		{
			memcpy(tmpRow, srcRow, rowSpan);
			srcRow += rowSpan;
			tmpRow -= rowSpan;
		}
		memcpy(dst.mData, tmpData, rowSpan * height);

		delete[] tmpData;
	}
}
void WIn32Window::windowMovedOrResized()
{
	if (!mHWnd || IsIconic(mHWnd))
		return;

	RECT rc;
	// top and left represent outer window position
	GetWindowRect(mHWnd, &rc);
	mtop = rc.top;
	mleft = rc.left;
	// width and height represent drawable area only
	GetClientRect(mHWnd, &rc);

	if (mWidth == rc.right && mHeight == rc.bottom)
		return;

	mWidth = rc.right - rc.left;
	mHeight = rc.bottom - rc.top;

	// Notify viewports of resize
	Viewportlist::iterator it, itend;
	itend = viewportPriorityList.end();
	for (it = viewportPriorityList.begin(); it != itend; ++it)
	{
		it->second->_updateDimensions();
	}
}
void WIn32Window::destroy()
{
	if (!mHWnd)
	{
		return;
	}
	delete mContext;

	if (!mIsExternalGLContext && mGlrc)
	{
		wglDeleteContext(mGlrc);
		mGlrc = 0;
	}
	if (!mIsExternal)
	{
		assert(0);
		if (mFullScreen)
		{
			ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);
		}
		DestroyWindow(mHWnd);
	}
	else
	{
		ReleaseDC(mHWnd, mHdc);
	}
	mActive = false;
	mClosed = true;
	mHWnd = 0;
	if (mDeviceName != NULL)
	{
		delete[] mDeviceName;
		mDeviceName = NULL;
	}
}

void WIn32Window::getCustomAttribute(const std::string& name, void* pData)
{
	if (name == "GLCONTEXT") {
		*static_cast<GLContext**>(pData) = mContext;
		return;
	}
	else if (name == "WINDOW")
	{
		HWND *pHwnd = (HWND*)pData;
		*pHwnd = getWindHandle();
		return;
	}
}

void WIn32Window::setActive(bool state)
{
	if (mDeviceName != NULL && state == false)
	{
		HWND hActiveWindow = GetActiveWindow();
		char classNameSrc[_MAX_CLASS_NAME_ + 1];
		char classNameDst[_MAX_CLASS_NAME_ + 1];

		GetClassName(mHWnd, classNameSrc, _MAX_CLASS_NAME_);
		GetClassName(hActiveWindow, classNameDst, _MAX_CLASS_NAME_);

		if (strcmp(classNameDst, classNameSrc) == 0)
		{
			state = true;
		}
	}

	mActive = state;

	if (mFullScreen)
	{
		if (state == false)
		{	//Restore Desktop
			ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);
			ShowWindow(mHWnd, SW_SHOWMINNOACTIVE);
		}
		else
		{	//Restore App
			ShowWindow(mHWnd, SW_SHOWNORMAL);

			DEVMODE displayDeviceMode;

			memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
			displayDeviceMode.dmSize = sizeof(DEVMODE);
			displayDeviceMode.dmBitsPerPel = mColourDepth;
			displayDeviceMode.dmPelsWidth = mWidth;
			displayDeviceMode.dmPelsHeight = mHeight;
			displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
			if (mDisplayFrequency)
			{
				displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
				displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
			}
			ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL);
		}
	}
}