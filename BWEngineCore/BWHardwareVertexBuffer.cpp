#include "BWHardwareVertexBuffer.h"
#include "BWColorValue.h"
#include "BWShadowHardwareBuffer.h"
BWHardwareVertexBuffer::BWHardwareVertexBuffer(BWHardwareBufferManagerBase *manager, size_t vertexSize, size_t vertexNum, BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer)
:BWHardwareBuffer(usage, false, useShadowBuffer), mVertexSize(vertexSize), mVertexNum(vertexNum)
{
	mSizeInBytes = mVertexSize * vertexNum;
	if (mUseShadowBuffer)
	{
		mShadowHardwareBuffer = new BWVertexShadowHardwareBuffer(manager, vertexSize, vertexNum, usage, false);
	}
}
size_t BWHardwareVertexBuffer::getVertexNum() const
{
	return mVertexNum;
}
size_t BWHardwareVertexBuffer::getVertexSize() const
{
	return mVertexSize;
}
VertexDeclearaion::VertexDeclearaion()
{

}
const VertexElement& VertexDeclearaion::addElement(unsigned short source, size_t offset, VertexElementType type, VertexElementSemantic sematic, unsigned short index /* = 0 */)
{
	if (type == VET_COLOR)
	{
		type = VertexElement::getBestColourVertexElementType();
	}
	mVertexElementList.push_back(VertexElement(source, offset, type, sematic, index));
	return mVertexElementList.back();
}
VertexDeclearaion::VertexElementList VertexDeclearaion::findElementBySource(int source)
{
	VertexElementList finalList;
	VertexElementList::iterator bi, ei;
	bi = mVertexElementList.begin();
	ei = mVertexElementList.end();
	for (; bi != ei; bi++)
	{
		if (bi->getSource() == source)
		{
			finalList.push_back(*bi);
		}
	}
	return finalList;
}
size_t VertexDeclearaion::getVertexSize(int source)
{
	VertexElementList::iterator ib, ie;
	size_t size = 0;
	ib = mVertexElementList.begin();
	ie = mVertexElementList.end();
	for (; ib != ie; ib++)
	{
		if (ib->getSource() == source)
		{
			size += ib->getSize();
		}
	}
	return size;
}
const VertexDeclearaion::VertexElementList&  VertexDeclearaion::getElements() const
{

	return mVertexElementList;
}
const VertexElement* VertexDeclearaion::getElement(int index)
{
	if (index >= mVertexElementList.size())
	{
		assert(0);
	}
	VertexElementList::iterator itor = mVertexElementList.begin();
	for (int i = 0; i <= index; i++)
	{
		itor++;
	}
	return &(*itor);
}
VertexElement::VertexElement(unsigned short source, size_t offset, VertexElementType type, VertexElementSemantic semantic, unsigned short index) :mSource(source),
mOffset(offset),
mType(type) ,
mVertexElementSemantic(semantic) ,
mIndex(index)
{

}
unsigned short VertexElement::getSource()  const
{
	return mSource;
}
VertexElementSemantic VertexElement::getVertexSemantic() const
{
	return mVertexElementSemantic;
}
int VertexElement::getOffset() const
{
	return mOffset;	
}
unsigned short  VertexElement::getIndex() const
{
	return mIndex;
}
size_t VertexElement::getSize()
{
	return getTypeSize(mType);
}
VertexElementType VertexElement::getType() const 
{
	return mType;
}
size_t VertexElement::getTypeSize(VertexElementType type)
{
	switch (type)
	{
	case VET_FLOAT1:
		return sizeof(float)* 1;
	case VET_FLOAT2:
		return sizeof(float)* 2;
	case VET_FLOAT3:
		return sizeof(float)* 3;
	case VET_FLOAT4:
		return sizeof(float)* 4;
	case VET_SHORT1:
		return sizeof(short)* 1;
	case VET_SHORT2:
		return sizeof(short)* 2;
	case VET_SHORT3:
		return sizeof(short)* 3;
	case VET_SHORT4:
		return sizeof(short)* 4;
	case VET_UBYTE4:
		return sizeof(unsigned char)* 4;
	case VET_COLOR:
	case VET_COLOUR_ARGB:
	case VET_COLOUR_ABGR:
		return sizeof(RGBA);
	default:
		return 0;
	}
}
size_t VertexElement::getTypeCount(VertexElementType type)
{
	switch (type)
	{
	case VET_FLOAT1:
		return 1;
	case VET_FLOAT2:
		return 2;
	case VET_FLOAT3:
		return 3;
	case VET_FLOAT4:
		return 4;
	case VET_COLOR:
	case VET_COLOUR_ARGB:
	case VET_COLOUR_ABGR:
		return 1;
	case VET_SHORT1:
		return 1;
	case VET_SHORT2:
		return 2;
	case VET_SHORT3:
		return 3;
	case VET_SHORT4:
		return 4;
	case VET_UBYTE4:
		return 4;
	}
	assert(0);
}
size_t VertexElement::getBaseType(VertexElementType type)
{
	switch (type)
	{
	case VET_FLOAT1:
	case VET_FLOAT2:
	case VET_FLOAT3:
	case VET_FLOAT4:
		return VET_FLOAT1;
	case VET_COLOR:
		return VET_COLOR;
	case VET_SHORT1:
	case VET_SHORT2:
	case VET_SHORT3:
	case VET_SHORT4:
		return VET_SHORT1;
	case VET_UBYTE4:
		return VET_UBYTE4;
	case VET_COLOUR_ARGB:
		return VET_COLOUR_ARGB;
	case VET_COLOUR_ABGR:
		return VET_COLOUR_ABGR;
	default:
		break;
	}
	return VET_FLOAT1;
}
VertexElementType VertexElement::getBestColourVertexElementType()
{
	// 根据渲染系统来确定使用哪个？
	return VET_COLOUR_ABGR;
}
void VertexBufferBinding::setBinding(unsigned short source, BWHardwareVertexBufferPtr &buffer)
{
	VertexBufferBindMap::iterator itor = mVertexBufferBindMap.find(source);
	if (itor != mVertexBufferBindMap.end())
	{
		assert(0);
	}
	mVertexBufferBindMap[source] = buffer;
}
bool VertexBufferBinding::isBufferBound(unsigned short index) const
{
	VertexBufferBindMap::const_iterator itor = mVertexBufferBindMap.find(index);
	return itor != mVertexBufferBindMap.end() ? true : false;
}
int VertexBufferBinding::getBufferNum() const
{
	return mVertexBufferBindMap.size();
}
BWHardwareVertexBufferPtr VertexBufferBinding::getBuffer(unsigned short index) const
{
	if (isBufferBound(index))
	{
		VertexBufferBindMap::const_iterator itor = mVertexBufferBindMap.find(index);
		return itor->second;
	}
	return NULL;
}
