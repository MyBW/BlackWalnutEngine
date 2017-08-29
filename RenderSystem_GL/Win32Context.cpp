#include "Win32Context.h"
#include <assert.h>
Win32Context::Win32Context(HDC hdc, HGLRC glrc) :mHDC(hdc), mGLrc(glrc)
{
}

Win32Context::~Win32Context()
{
}

void Win32Context::setCurrent()
{
	wglMakeCurrent(mHDC, mGLrc);
}
void Win32Context::endCurrent()
{
	wglMakeCurrent(NULL, NULL);
}

GLContext* Win32Context::clone() const
{
	HGLRC newCtc = wglCreateContext(mHDC);
	if (!newCtc)
	{
		assert(0);
	}

	HGLRC old = wglGetCurrentContext();
	HDC oldHdc = wglGetCurrentDC();
	wglMakeCurrent(NULL, NULL);
	if (!wglShareLists(mGLrc , newCtc))
	{
		//std::string errorMessage = win32sub
		assert(0);
	}
	wglMakeCurrent(oldHdc, old);

	return new Win32Context(mHDC, newCtc);
}
void Win32Context::releaseContext()
{
	if (mGLrc != NULL)
	{
		wglDeleteContext(mGLrc);
		mGLrc = NULL;
		mHDC = NULL;
	}
}