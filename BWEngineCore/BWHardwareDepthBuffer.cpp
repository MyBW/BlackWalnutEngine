#include "BWHardwareDepthBuffer.h"
#include "Math.h"
BWHardwareDepthBuffer::BWHardwareDepthBuffer(const std::string& Name, size_t width, size_t height, size_t depth,
	BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareBuffer(Name,usage, useSystemMemory, useShadowBuffer)
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

void BWHardwareDepthBuffer::SetHardwareBufferMipmap(int InMipmapLevelNum)
{
	MipmapLevelNum = InMipmapLevelNum;
	SetBufferMipmapImp(InMipmapLevelNum);
}

