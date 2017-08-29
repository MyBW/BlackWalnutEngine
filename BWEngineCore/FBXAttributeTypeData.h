#pragma once
#include "FBXImporterImp.h"

#define REGISTER_FBXIMPORT(AttributeType , CreateFunction)\
struct RegisterImp\
{\
	RegisterImp()\
	{\
		FBXImporterImp::GetAllCreateFunctionType().insert(FBXImporterImp::AllCreateFunctionType::value_type(AttributeType, std::bind(CreateFunction)));\
	}\
};\
static RegisterImp Register

#define REGISTER_FBXIMPORT_IMP(Import) Import::RegisterImp Import::Register 

const int TRIANGLE_VERTEX_COUNT = 3;
// Four floats for every position.
const int VERTEX_STRIDE = 4;
// Three floats for every normal.
const int NORMAL_STRIDE = 3;
// Four floats for every tangent.
const int TANGENT_STRIDE = 4;
// Two floats for every UV.
const int UV_STRIDE = 2;

#define ONELEMENTSIZE() VERTEX_STRIDE + NORMAL_STRIDE + TANGENT_STRIDE + UV_STRIDE 

class FBXAttributeBaseImport
{
public:
	//static typelist<FBXMeshAttribute*> typeList;
	virtual void ImportNodeAttribute(FbxNode* Node) { }
	std::function<void(std::string, std::string)> PrintLog;
};

class FBXMeshAttributeImport : public FBXAttributeBaseImport
{
public:
	REGISTER_FBXIMPORT(FbxNodeAttribute::EType::eMesh, &FBXMeshAttributeImport::CreateMeshImport);

	struct SubMesh
	{
		SubMesh() : IndexOffset(0), TriangleCount(0) {}

		int IndexOffset;
		int TriangleCount;
	};
	static FBXAttributeBaseImport* CreateMeshImport() { return new FBXMeshAttributeImport(); }
	void ImportNodeAttribute(FbxNode* Node) override;
	inline const std::vector<float>& GetVertices() const { return lVertices; }
	inline const std::vector<float>& GetUVs() const { return lUVs; }
	inline const std::vector<float>& GetNormals() const { return lNormals; }
	inline const std::vector<float>& GetTangents() const { return lTangents; }
	inline const std::vector<unsigned int>& GetIndices() const { return lIndices; }
	inline const int GetVertexElementSize() const { return ONELEMENTSIZE(); }
	inline const int GetVertexElementSizeByByte() const { return GetVertexElementSize() * sizeof(float); }
	inline const std::vector<FBXMeshAttributeImport::SubMesh*>& GetSubMeshs() const { return mSubMeshes; }
	~FBXMeshAttributeImport();
private:
	
	std::vector<SubMesh*> mSubMeshes;
	std::vector<float> lVertices;
	std::vector<float> lTangents;
	std::vector<unsigned int> lIndices;
	std::vector<float> lNormals;
	std::vector<float> lUVs;
};
