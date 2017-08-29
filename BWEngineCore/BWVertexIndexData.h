#ifndef VERTEXINDEXDATA_H_
#define VERTEXINDEXDATA_H_
#include "BWHardwareVertexBuffer.h"
#include "BWHardwareIndexBuffer.h"
class VertexData
{
	friend class BWMeshSerializerImp;
	friend class BWRenderSystem;
public:
	VertexData();
	~VertexData();

	int mVertexStart;
	unsigned int mVertexCount;
	VertexDeclearaion *mVertexDeclaration;
	VertexBufferBinding *mVertexBufferBind;
};
typedef SmartPointer<VertexData> VertexDataPrt;

class IndexData
{
	friend class BWMeshSerializerImp;
	friend class BWRenderSystem;
public:
	IndexData();
	~IndexData();

	int mIndexStart;
	unsigned int mIndexCount;
	BWHardwareIndexBufferPtr mIndexBuffer;
};
typedef SmartPointer<IndexData> IndexDataPrt;

#endif