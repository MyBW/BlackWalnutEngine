#pragma once
#include "BWHardwareBuffer.h"
#include "BWPixlFormat.h"
#include "BWSmartPointer.h"
#include "BWResource.h"


class BWImageTextureBuffer : public BWHardwareBuffer
{
public:
	BWImageTextureBuffer(const std::string& name, PixelFormat Format, Usage usage, bool systemMemory, bool isUseShadowBuffer)
		:BWHardwareBuffer(name, usage, systemMemory, isUseShadowBuffer)
	{
		this->Format = Format;
	}
	virtual ~BWImageTextureBuffer(){ }
	void SetIndex(int InIndex) { Index = InIndex; }
	int GetIndex() const { return Index; }
protected:
	PixelFormat Format;
	int Index;
};


class BWImageTexturebufferPtr : public SmartPointer<BWImageTextureBuffer>
{
public:
	BWImageTexturebufferPtr() :SmartPointer<BWImageTextureBuffer>()
	{

	}
	BWImageTexturebufferPtr(const BWResourcePtr & resource)
	{
		mPointer = dynamic_cast<BWImageTextureBuffer*>(resource.Get());
		counter = resource.GetCounterPointer();
		if (counter)
		{
			(*counter)++;
		}
	}
	const BWImageTexturebufferPtr& operator=(const BWResourcePtr &resource)
	{
		if (mPointer == dynamic_cast<BWImageTextureBuffer*>(resource.Get()))
		{
			return *this;
		}
		if (mPointer != NULL)
		{
			(*counter)--;
			if ((*counter) == 0)
			{
				delete mPointer;
			}
		}
		mPointer = dynamic_cast<BWImageTextureBuffer*>(resource.Get());
		counter = resource.GetCounterPointer();
		(*counter)++;
		return *this;
	}
};