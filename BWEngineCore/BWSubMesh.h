#ifndef SUBMESH_H_
#define SUBMESH_H_
#include "BWVertexIndexData.h"
#include "BWMeshSerializerImp.h"
#include "BWRenderOperation.h"
#include "BWCommon.h"
#include <vector>

class BWSubMesh
{
	friend class BWMeshSerializerImp;
	friend class BWSubEntity;
public:
	BWSubMesh();
	~BWSubMesh();
	bool isMatInitialised();
	void setMaterialName(const std::string &materialName, const std::string &groupName = DEFAULT_RESOURCE_GROUP);
	const std::string & getMaterialName();
	void addTextureAlias(const std::string &aliasName, const std::string &textureName);
	void setParent(BWMesh *parent);
	BWMesh* getParent(){ return mParent; }
	VertexAnimationType getVertextAnimationType() { return VAT_NONE; }
	void _getRenderOperation(BWRenderOperation &ro);
	VertexDataPrt getVertexData() const;
	IndexDataPrt  getIndexData() const;
	void SetVertexData(VertexDataPrt InVertexData) { mVertexData = InVertexData; }
	void SetIsUseSharedVertex(bool InIsUseSharedVertex) { mIsUseSharedVertex = InIsUseSharedVertex; }
private:
	typedef std::vector<IndexData*> LodFaceList;
	typedef std::map<std::string, std::string> AliasNameTexutreNameMap;
	VertexDataPrt mVertexData;
	IndexDataPrt  mIndexData;

	LodFaceList mLodFaceList;
	std::string mMaterialName;
	bool  mIsHaveMaterial;
	bool  mIsUseSharedVertex;
	BWMesh *mParent;
	BWRenderOperation::OperatinType mOperationType;
	AliasNameTexutreNameMap mAliasNameTextureNameMap;
};

#endif