#include "BWDemoMeshImport.h"
#include "BWHardwareVertexBuffer.h"
#include "BWHardwareBufferManager.h"
#include "BWMesh.h"
#define CURRENT_DEMO_MESH_VERSION 1

bool BWDemoMeshImport::Load(std::string FileName, BWMesh* Mesh)
{

	std::map<std::string, std::string> Material;
	Material["materials/sponza/arch.mtl"] = "ArchMaterial";
	Material["materials/sponza/bricks.mtl"] = "Bricks";
	Material["materials/sponza/ceiling.mtl"] = "Ceiling";
	Material["materials/sponza/chain.mtl"] = "Chain";
	Material["materials/sponza/column_a.mtl"] = "Column_a";
	Material["materials/sponza/column_b.mtl"] = "Column_b";
	Material["materials/sponza/column_c.mtl"] = "Column_c";
	Material["materials/sponza/details.mtl"] = "Details";
	Material["materials/sponza/fabric_a.mtl"] = "Fabric_a";
	Material["materials/sponza/fabric_c.mtl"] = "Fabric_c";
	Material["materials/sponza/fabric_d.mtl"] = "Fabric_d";
	Material["materials/sponza/fabric_e.mtl"] = "Fabric_e";
	Material["materials/sponza/fabric_f.mtl"] = "Fabric_f";
	Material["materials/sponza/fabric_g.mtl"] = "Fabric_g";
	Material["materials/sponza/flagpole.mtl"] = "Flagpole";
	Material["materials/sponza/floor.mtl"] = "Floor";
	Material["materials/sponza/leaf.mtl"] = "Leaf";
	Material["materials/sponza/Material__25.mtl"] = "Material__25";
	Material["materials/sponza/Material__47.mtl"] = "Material__47";
	Material["materials/sponza/Material__57.mtl"] = "Material__57";
	Material["materials/sponza/Material__298.mtl"] = "Material__298";
	Material["materials/sponza/roof.mtl"] = "Roof";
	Material["materials/sponza/vase.mtl"] = "Vase";
	Material["materials/sponza/vase_hanging.mtl"] = "Vase_Hanging";
	Material["materials/sponza/vase_round.mtl"] = "Vase_Round";
	char CurrentFileName[50];
	strcpy(CurrentFileName, FileName.data());
	std::string FilePath = CurrentFileName;
	FILE *File;
	fopen_s(&File, FilePath.data(), "rb");
	if (!File)
		return false;

	// check idString
	char idString[5];
	memset(idString, 0, 5);
	fread(idString, sizeof(char), 4, File);
	if (strcmp(idString, "DMSH") != 0)
	{
		fclose(File);
		return false;
	}

	// check version
	unsigned int version;
	fread(&version, sizeof(unsigned int), 1, File);
	if (version != CURRENT_DEMO_MESH_VERSION)
	{
		fclose(File);
		return false;
	}

	// get number of vertices
	unsigned int numVertices;
	fread(&numVertices, sizeof(unsigned int), 1, File);
	if (numVertices < 3)
	{
		fclose(File);
		return false;
	}
	struct GeometryVertex
	{
		float position[3];
		float texCoords[2];
		float normal[3];
		float tangent[4];
	};
	// load vertices
	GeometryVertex *vertices = new GeometryVertex[numVertices];
	if (!vertices)
	{
		fclose(File);
		return false;
	}
	fread(vertices, sizeof(GeometryVertex), numVertices, File);
	std::string VerticesNum = std::to_string(numVertices);
	Log::GetInstance()->logMessage( std::string("VerticesNum: ") +  VerticesNum);

	// get number of indices
	unsigned int numIndices;
	fread(&numIndices, sizeof(unsigned int), 1, File);
	if (numIndices < 3)
	{
		fclose(File);
		return false;
	}

	// load indices 
	unsigned int *indices = new unsigned int[numIndices];
	if (!indices)
	{
		fclose(File);
		return false;
	}
	fread(indices, sizeof(unsigned int), numIndices, File);
	VerticesNum = std::to_string(numIndices);
	Log::GetInstance()->logMessage(std::string("numIndices: ") + VerticesNum);

	BWHardwareVertexBufferPtr vbuf;
	int VertexByteSize = sizeof(GeometryVertex);
	vbuf = BWHardwareBufferManager::GetInstance()->createVertexBuffer(FileName,
		VertexByteSize, numVertices,
		BWHardwareBuffer::Usage::HBU_DYNAMIC, false
	);
	void * buf = vbuf->lock(BWHardwareBuffer::HBL_DISCARD);
	memcpy(buf, vertices, numVertices * VertexByteSize);
	vbuf->unlock();
	VertexDataPrt SharedVertexData = new VertexData();
	Mesh->SetVertexData(SharedVertexData);
	SharedVertexData->mVertexStart = 0;
	SharedVertexData->mVertexCount = numVertices;
	SharedVertexData->mVertexDeclaration->addElement(0, 0 * sizeof(float), VET_FLOAT3, VES_POSITION);
	SharedVertexData->mVertexDeclaration->addElement(0, 3 * sizeof(float), VET_FLOAT2, VES_TEXTURE_COORDINATES);
	SharedVertexData->mVertexDeclaration->addElement(0, 5 * sizeof(float), VET_FLOAT3, VES_NORMAL);
	SharedVertexData->mVertexDeclaration->addElement(0, 8 * sizeof(float), VET_FLOAT4, VES_TANGENT);
	SharedVertexData->mVertexBufferBind->setBinding(0, vbuf);

	BWHardwareIndexBufferPtr ibuf;
	ibuf = BWHardwareBufferManager::GetInstance()->createIndexBuffer(Mesh->getName(),
		BWHardwareIndexBuffer::IT_32BIT,
		numIndices,
		BWHardwareBuffer::Usage::HBU_DYNAMIC,
		false);
	unsigned int *pIdx = static_cast<unsigned int*>(ibuf->lock(BWHardwareBuffer::HBL_DISCARD));
	memcpy(pIdx, indices, numIndices * sizeof(unsigned int));
	ibuf->unlock();

	// get number of sub-meshes
	unsigned int numSubMeshes;
	fread(&numSubMeshes, sizeof(unsigned int), 1, File);
	if (numSubMeshes < 1)
	{
		fclose(File);
		return false;
	}
	// load/ create sub-meshes
	for (unsigned int i = 0; i < numSubMeshes; i++)
	{
		BWSubMesh* SubMesh = Mesh->createSubMesh();
		SubMesh->SetIsUseSharedVertex(true);
		SubMesh->getIndexData()->mIndexBuffer = ibuf;
		if (!SubMesh)
		{
			fclose(File);
			return false;
		}
		char materialName[256];
		fread(materialName, sizeof(char), 256, File);
		std::string material = Material[materialName];
		SubMesh->setMaterialName(material);
		fread(&SubMesh->getIndexData()->mIndexStart, sizeof(unsigned int), 1, File);
		VerticesNum = std::to_string(SubMesh->getIndexData()->mIndexStart);
		Log::GetInstance()->logMessage(std::string("mIndexStart: ") + VerticesNum);

		fread(&SubMesh->getIndexData()->mIndexCount, sizeof(unsigned int), 1, File);
		VerticesNum = std::to_string(SubMesh->getIndexData()->mIndexCount);
		Log::GetInstance()->logMessage(std::string("mIndexCount: ") + VerticesNum);
       if(i == 20) break;
	}
	delete[] indices;
	delete[] vertices;
	fclose(File);
	return true;
}

