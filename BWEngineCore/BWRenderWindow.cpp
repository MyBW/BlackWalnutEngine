#include "BWRenderWindow.h"

BWRenderWindow::BWRenderWindow() :BWRenderTarget(), mIsPrimary(false)
{
	mAutoDeactivateOnFocusChange = false;
}

BWRenderWindow::~BWRenderWindow()
{
}
bool BWRenderWindow::isDeactivateOnFocusChange() const 
{
	return mAutoDeactivateOnFocusChange;
}
void BWRenderWindow::getMetrics(unsigned int &width, unsigned int &height, unsigned int & colorDepth, int &left, int &top)
{
	width = mWidth;
	height = mHeight;
	colorDepth = mColorDepth;
	left = mleft;
	top = mtop;
}