#include "BWSubMesh.h"
#include "BWMesh.h"
#include "BWMaterialManager.h"
BWSubMesh::BWSubMesh() :mVertexData(NULL), mParent(NULL), mIsUseSharedVertex(false), mOperationType(BWRenderOperation::OT_TRIANGLE_LIST)
{
	mIndexData = new IndexData();
}
BWSubMesh::~BWSubMesh()
{
	if (mVertexData.IsNull())
	{
	    mVertexData.SetNull();
	}
	mIndexData.SetNull();
	LodFaceList::iterator itor = mLodFaceList.begin();
	LodFaceList::iterator itore = mLodFaceList.end();
	for (; itor != itore ; itor++)
	{
		delete (*itor);
	}
	mLodFaceList.clear();
}
void BWSubMesh::setParent(BWMesh *parent)
{
	mParent = parent;
}
bool BWSubMesh::isMatInitialised()
{
	return mIsHaveMaterial;
}
void BWSubMesh::setMaterialName(const std::string &materialName, const std::string &groupName)
{
	mMaterialName = materialName;
	mIsHaveMaterial = true;
}
const std::string& BWSubMesh::getMaterialName()
{
	return mMaterialName;
}

void BWSubMesh::addTextureAlias(const std::string &aliasName, const std::string &textureName)
{
	mAliasNameTextureNameMap[aliasName] = textureName;
}
void BWSubMesh::_getRenderOperation(BWRenderOperation &ro)
{
	ro.useIndexes = mIndexData->mIndexCount != 0;
	ro.indexData = mIndexData;
	ro.operationType = mOperationType;
	ro.vertexData = mIsUseSharedVertex ? mParent->sharedVertexData : mVertexData;
}
VertexDataPrt BWSubMesh::getVertexData() const
{
	return mVertexData;
}
IndexDataPrt BWSubMesh::getIndexData() const
{
	return mIndexData;
}