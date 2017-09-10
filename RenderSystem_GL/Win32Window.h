#ifndef WIN32WINDOW_H_
#define WIN32WINDOW_H_
#include "../BWEngineCore/BWRenderWindow.h"
#include "../BWEngineCore/BWCommon.h"
#include"../BWEngineCore/BWMaterial.h"
#include"GLTexture.h"
#include "GLWin32Support.h"
#include "Win32Context.h"
#include "../BWEngineCore/BWGpuProgramUsage.h"

class WIn32Window : public BWRenderWindow
{
public:
	WIn32Window(GLWin32Support &glsupport);
	~WIn32Window();
	void create(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList *miscParams = 0);
	void adjustWindow(unsigned int clientWidth, unsigned int clientHeight,
		unsigned int* winWidth, unsigned int* winHeight);
	void destroy();
	void setFullScreen(bool fullScreen, unsigned int width, unsigned int height);
	bool isActive() const;
	bool isVisible() const;
	bool isClosed() const;
	void reposition(int left, int top);
	void resize(unsigned int width, unsigned int height);
	void swapBuffers(bool waritForVSync = true);
	virtual void copyContentsToMemory(const BWPixelBox &dst, FrameBuffer buffer);
	void windowMovedOrResized();
	bool requiresTextureFlipping() const { return false; }
	HWND getWindHandle() const { return mHWnd; }
	HDC getHDC() const { return mHdc; }
	void getCustomAttribute(const std::string &name , void *data);
	void setActive(bool state);
	void rePosition(int left, int top);
	virtual void clearRenderTarget() override;
	bool InitWindow() override;
protected:
	//void updateImp();


	virtual BWHardwarePixelBufferPtr _createHardwarePixelBufferImp(const std::string &name, RenderBufferType bufferType, BWRenderTarget *creator) override { return NULL; };
	virtual BWHardwareStencilBufferPtr _createHardwareStencilBufferImp(const std::string &name, BWRenderTarget *creator) override { return NULL; };
	virtual BWHardwareDepthBufferPtr _createHardwareDepthBufferImp(const std::string &name, BWRenderTarget *creator) override { return NULL; };

	virtual bool _removeHardwarePixelBufferImp(BWHardwarePixelBufferPtr pixelBuffer) override { return false; };
	virtual bool _removeHardwareDepthBufferImp(BWHardwareDepthBufferPtr depthBuffer) override { return false; };
	virtual bool _removeHardwareStencilBufferImp(BWHardwareStencilBufferPtr stencilBuffer) override { return false; };
	virtual bool _removeTextureBufferImp(BWTexturePtr texture) override { return false; }
	virtual void _setRenderTarget() override;
	virtual void _endUpdate() override;
private:
	GLWin32Support *mSupport;
	Win32Context *mContext;
	HDC mHdc;
	HWND mHWnd;
	HGLRC mGlrc;
	bool mIsExternal;
	bool mIsExternalGLControl;
	bool mIsExternalGLContext;
	bool mSizing;
	bool mClosed;
	int mDisplayFrequency;
	char *mDeviceName;


	// 这里是延迟渲染相关的东西 但是这里涉及到了GL相关的代码 不应该这样随后调整
	BWMaterialPtr material;
	class GLSLGpuProgram* glslProgram;
	BWGpuProgramUsagePtr  gpuPrgramUsage;
	GLuint BufferID ;
	GLint BaseColorMapLoc;
	GLint NormalMapLoc;
	GLint PositionMapLoc;

	int SphereVertexNum;
	BWMaterialPtr mPointLightDefferLightingMaterial;

	class GLSLGpuProgram* mPointLightDefferLightingGLSLProgram;
	BWGpuProgramUsagePtr  mPointLightDefferLightingGpuPrgramUsage;
	GLuint mPointLightDefferLightingBufferID;
	GLuint mPointLightDefferLightingIndexBufferID;

	/*GLint mPointLightDefferLightingBaseColorMapLoc;
	GLint mPointLightDefferLightingNormalMapLoc;
	GLint mPointLightDefferLightingPositionMapLoc;*/

	GLTexturePtr BaseColorTexture;
	GLTexturePtr NormalTexture;
	GLTexturePtr PositionTexture;
	class BWRenderTarget* GBuffer;
};


#endif