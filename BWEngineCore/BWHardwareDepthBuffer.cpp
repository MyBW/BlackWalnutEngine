#include "BWHardwareDepthBuffer.h"
#include "Math.h"
BWHardwareDepthBuffer::BWHardwareDepthBuffer(size_t width, size_t height, size_t depth,
	BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareBuffer(usage, useSystemMemory, useShadowBuffer)
	, mWidth(width)
	, mHeight(height)
	, mDepth(depth)
	,mRenderTarget(NULL)
	,IsWithStencil(true)
{

}

void BWHardwareDepthBuffer::SetHardwareBufferSize(int Width, int Height)
{
	mWidth = TMax(Width, 1);
	mHeight = TMax(Height, 1);
	SetBufferSizeImp(mWidth, mHeight);
}

