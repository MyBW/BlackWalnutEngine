#include "BWHardwareStencilBuffer.h"

BWHardwareStencilBuffer::BWHardwareStencilBuffer(size_t width, size_t height, size_t depth,
	BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
	:BWHardwareBuffer(usage, useSystemMemory, useShadowBuffer),
    mWidth(width),
    mHeight(height),
    mDepth(depth)
{

}