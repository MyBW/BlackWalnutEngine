#include "BWMesh.h"
#include "BWResourceGroupMananger.h"
#include "BWMeshSerializer.h"
#include "BWFBXImporter.h"
BWMesh::BWMesh(BWResourceManager* creator, const std::string &name, const std::string &groupName) :BWResource(creator, name, groupName)
, mIsLodManual(false)
, mNumLods(0)
, mVertexBufferShadowed(false) 
, mIndexBufferShadowed(false)
, sharedVertexData(NULL)
{

}
void BWMesh::setVertexBufferUsage(BWHardwareBuffer::Usage vertexBufferUsage, bool vertextBufferShadowed)
{
	mVertextBufferUsage = vertexBufferUsage;
	mVertexBufferShadowed = vertextBufferShadowed;
}
void BWMesh::setIndexBufferUsage(BWHardwareBuffer::Usage indexBufferUsage, bool indexBufferShadowed)
{
	mIndexBufferUsage = indexBufferUsage;
	mIndexBufferShadowed = indexBufferShadowed;
}
void BWMesh::loadImpl()
{
	BWDataStreamPrt tmpDataStream(mFreshFromDisk);
	mFreshFromDisk.SetNull();
	if (tmpDataStream.IsNull())
	{
		assert(0);
	}
	std::string Name = "ogrehead.mesh";
	if (Name == name)
	{
		BWFBXImporter Import;
		std::string FBXFileName = "SK_Mannequin.FBX";
		Import.ImportFBX(FBXFileName, this);
	}
	else
	{
		BWMeshSerializer serializer;
		serializer.importMesh(tmpDataStream, this);
	}
	
	updateMaterialForAllSubMesh();
}
void BWMesh::prepareImpl()
{
	mFreshFromDisk = BWResourceGroupManager::GetInstance()->OpenResource(name, groupName);
	//这里可以将mFreamFromDisk从文件流操作 转换为 内存操作（MemoryDataStream）
}
BWSubMesh* BWMesh::getSubMesh(unsigned int index)
{
	if (index < mSubMeshList.size())
	{
		return mSubMeshList[index];
	}
	return NULL;
}
void BWMesh::updateMaterialForAllSubMesh()
{

}
void BWMesh::nameSubMesh(const std::string &name, unsigned short index)
{
	mSubMeshNamedMap[name] = index;
}
unsigned short BWMesh::getNumSubMeshes()
{
	return mSubMeshList.size();
}
BWSubMesh * BWMesh::createSubMesh()
{
	BWSubMesh *tmp = new BWSubMesh();
	mSubMeshList.push_back(tmp);
	tmp->setParent(this);
	return tmp;
}