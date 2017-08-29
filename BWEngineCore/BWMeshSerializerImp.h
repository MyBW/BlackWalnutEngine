#ifndef MESHSERIALIZERIMP_H_
#define MESHSERIALIZERIMP_H_
#include "BWDataStream.h"
#include "BWMaterial.h"
#include "BWSerializer.h"
#include "BWVertexIndexData.h"
class BWMesh;
class BWSubMesh;
class BWMeshSerializerImp : public BWSerializer
{
	
public:
	void importMesh(BWDataStreamPrt &dataStream, BWMesh *mesh);
protected:
	virtual void readTextureLayer(BWDataStreamPrt &dataStream, BWMesh *mesh, BWMaterialPtr &material);
	virtual void readMesh(BWDataStreamPrt &dataStream, BWMesh *mesh);
	virtual void readGeometry(BWDataStreamPrt &dataStream, BWMesh *mesh , VertexData *vertexData);
	virtual void readSubMesh(BWDataStreamPrt &dataStream, BWMesh *mesh);
	virtual void readGeometryVertexDeclaration(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData);
	virtual void readGeometryVertexelement(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData);
	virtual void readGeometryVertexBuffer(BWDataStreamPrt &dataStream, BWMesh *mesh, VertexData *vertexData);
	virtual void readMeshLodInfo(BWDataStreamPrt& stream, BWMesh* pMesh);
	virtual void readBoundsInfo(BWDataStreamPrt& stream, BWMesh* pMesh);
	virtual void readSubMeshNameTable(BWDataStreamPrt& stream, BWMesh* pMesh);
	virtual void readEdgeList(BWDataStreamPrt& stream, BWMesh* pMesh);
	virtual void BWMeshSerializerImp::readEdgeListLodInfo(BWDataStreamPrt& stream /*EdgeData* edgeData*/);
	void BWMeshSerializerImp::readSubMeshBoneAssignment(BWDataStreamPrt &dataStream, BWMesh *mesh, BWSubMesh *subMesh);
	void BWMeshSerializerImp::readSubMeshOperation(BWDataStreamPrt &dataStream, BWMesh *mesh, BWSubMesh *subMesh);
	void BWMeshSerializerImp::readSubMeshTextureAlias(BWDataStreamPrt &dataStream, BWMesh *mesh, BWSubMesh *subMesh);
	virtual void readMeshLodUsageManual(BWDataStreamPrt& stream, BWMesh* pMesh, unsigned short lodNum);
	virtual void readMeshLodUsageGenerated(BWDataStreamPrt& stream, BWMesh* pMesh, unsigned short lodNum);
	virtual void flipFromLittleEndian(void *data, size_t vertexCounter, size_t vertexSize, const VertexDeclearaion::VertexElementList & elementList);
	virtual void flipEndian(void *data, size_t vertexCount, size_t vertexSize, const VertexDeclearaion::VertexElementList &elem);
private:
};

#endif