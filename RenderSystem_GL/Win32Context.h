#ifndef WIN32CONTEXT_H_
#define WIN32CONTEXT_H_
#include <windows.h>
#include "GLContext.h"
class Win32Context : public GLContext
{
public:
	Win32Context(HDC hdc , HGLRC GLrc);
	virtual ~Win32Context();
	virtual void setCurrent();
	virtual void endCurrent();
	GLContext *clone() const;
	virtual void releaseContext();
private:
	HDC mHDC;
	HGLRC mGLrc;

};

#endif