#ifndef RENDERTEXTURE_H_
#define RENDERTEXTURE_H_
#include "BWRenderTarget.h"
#include "BWPrimitive.h"
class BWHardwarePixelBuffer;

class BWRenderTexture : public BWRenderTarget
{
public:
	BWRenderTexture(BWHardwarePixelBuffer*hardwarePixelBuffer, size_t zoffset);
	virtual ~BWRenderTexture();
	virtual void copyContentsToMemory(const BWPixelBox &dst, FrameBuffer buffer);
	PixelFormat suggestPixelFormat() const;
protected:
	BWHardwarePixelBuffer *mBuffer;
	size_t mZoffset;
};

class BWMultiRenderTarget : public BWRenderTarget
{
public:
	typedef std::vector<BWRenderTexture*> BoundSurfaceList;
	BWMultiRenderTarget(const std::string &name);
	virtual void bindSurface(size_t attachment, BWRenderTexture *target)
	{
		for (size_t  i = mBoundSurfaces.size(); i < attachment; i++)
		{
			mBoundSurfaces.push_back(0);
		}
		mBoundSurfaces[attachment] = target;
		bindSurfaceImp(attachment , target);
	}
	virtual void unbindSurface(size_t attachment)
	{
		if (attachment < mBoundSurfaces.size())
		{
			mBoundSurfaces[attachment] = 0;
		}
		unbindSurfaceImp(attachment); 
	}
	virtual void copyContentsToMemory(const BWPixelBox &dst, FrameBuffer frame);
	PixelFormat suggestPixelFormat() const { return PF_UNKNOWN; }
	const BoundSurfaceList& getBoundSurfaceList() const { return mBoundSurfaces; }
protected:
	virtual void bindSurfaceImp(size_t attachment , BWRenderTarget *target)= 0;
	virtual void unbindSurfaceImp(size_t attachment) = 0;
	BoundSurfaceList mBoundSurfaces;
};

#endif