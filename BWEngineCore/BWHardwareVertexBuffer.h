#ifndef HARDWAREVERTEXBUFFER_H_
#define HARDWAREVERTEXBUFFER_H_
#include "BWHardwareBuffer.h"
#include "BWSmartPointer.h"
#include <list>
#include <map>
class BWHardwareBufferManagerBase;
class BWHardwareVertexBuffer : public BWHardwareBuffer
{
public:
	BWHardwareVertexBuffer(BWHardwareBufferManagerBase *manager,const std::string& Name, size_t vertexSize, size_t vertexNum,
		BWHardwareBuffer::Usage usage, bool useSystemMemory, bool useShadowBuffer);
	size_t getVertexSize() const;
	size_t getVertexNum() const;
	BWHardwareBufferManagerBase * getManager();
protected:
private:
	BWHardwareBufferManagerBase *mManager;
	size_t mVertexSize;
	size_t mVertexNum;
};
typedef SmartPointer<BWHardwareVertexBuffer> BWHardwareVertexBufferPtr;

enum VertexElementType 
{
	VET_FLOAT1 = 0 ,
	VET_FLOAT2 = 1,
	VET_FLOAT3 = 2 ,
	VET_FLOAT4 = 3 ,
	VET_COLOR = 4 ,
	VET_SHORT1 = 5 ,
	VET_SHORT2 = 6 ,
	VET_SHORT3 = 7 ,
	VET_SHORT4 = 8 ,
	VET_UBYTE4 = 9,
	VET_COLOUR_ARGB = 10 ,
	VET_COLOUR_ABGR = 11
};

enum VertexElementSemantic
{
	VES_POSITION = 1,
	VES_BLEND_WEIGHTS = 2,
	VES_BLEND_INDICES = 3,
	VES_NORMAL = 4,
	VES_DIFFUSE = 5,
	VES_SPECULAR = 6 ,
	VES_TEXTURE_COORDINATES = 7 ,
	VES_BINORMAL =8 ,
	VES_TANGENT = 9	 
};
class VertexBufferBinding
{
public:
	typedef std::map<unsigned int, BWHardwareVertexBufferPtr> VertexBufferBindMap;
	virtual void setBinding(unsigned short source, BWHardwareVertexBufferPtr &buffer);
	bool isBufferBound(unsigned short index) const;
	BWHardwareVertexBufferPtr getBuffer(unsigned short index) const;
	int getBufferNum() const;
	const VertexBufferBindMap& GetVertexBufferBindMap() { return mVertexBufferBindMap; }
protected:
	VertexBufferBindMap mVertexBufferBindMap;
	mutable unsigned short mHighIndex;
private:
};
class VertexElement
{
public:
	VertexElement(unsigned short source, size_t offset, VertexElementType type, VertexElementSemantic semantic, unsigned short index);
	inline void baseVertexPointerToElement(void *base, void ** pElem)  const
	{
		*pElem = static_cast<void *> (static_cast<unsigned char*>(base)+mOffset);
	}
	static size_t getBaseType(VertexElementType type);
	static VertexElementType getBestColourVertexElementType();
	static size_t getTypeCount(VertexElementType type);
	static size_t getTypeSize(VertexElementType type);
	VertexElementType getType() const ;
	unsigned short getSource() const;
	VertexElementSemantic getVertexSemantic() const;
	int getOffset() const;
	size_t getSize();
	unsigned short getIndex() const;
protected:
private:
	int mOffset;
	unsigned short mSource; // 这个source 就是 vertexbuffer的序号  一个mesh文件有多个vertexbuffer 
	VertexElementType mType;
	VertexElementSemantic mVertexElementSemantic;
	unsigned short mIndex;
};
class VertexDeclearaion
{
public:
	typedef std::list<VertexElement> VertexElementList;
protected:
	VertexElementList mVertexElementList;
public:
	VertexDeclearaion();
	~VertexDeclearaion();
	size_t  getElementCount();
	const VertexElementList& getElements() const;
	const VertexElement* getElement(int index);
	size_t getVertexSize(int source);
	virtual const VertexElement& addElement(unsigned short source, size_t offset, VertexElementType type,
		VertexElementSemantic sematic, unsigned short index = 0);
	VertexElementList findElementBySource(int source);//这里返回的list里存放的是实例 而不是指针 会不会太浪费
};
#endif