#include "BWMeshManager.h"
#include "BWMesh.h"
BWMeshManager* BWMeshManager::GetInstance()
{
	assert(instance);
	return instance;
}
BWMeshManager::BWMeshManager()
{
	resourceType = "mesh";
}
BWMeshManager::~BWMeshManager()
{

}
BWResourceManager::ResourceCreateOrRetrieveResult BWMeshManager::CreateOrRetrieve(const std::string &name, const std::string &groupName,
	BWHardwareBuffer::Usage vertexBufferUsage /* = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY */,
	BWHardwareBuffer::Usage indexBufferUsage /* = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY */,
	bool vertexbufferShadowed /* = true */, bool indexBufferShadowed /* = true */)
{
	ResourceCreateOrRetrieveResult ret = BWResourceManager::CreateOrRetrieve(name, groupName, NULL);
	if (ret.second)
	{
		BWMeshPrt mesh = ret.first;
		mesh->setIndexBufferUsage(indexBufferUsage , indexBufferShadowed);
		mesh->setVertexBufferUsage(vertexBufferUsage , vertexbufferShadowed);
	}
	return ret;
}
BWResourcePtr BWMeshManager::CreateImp(const std::string &name , const std::string &groupName)
{
	return new BWMesh(this, name, groupName);
}
BWMeshPrt BWMeshManager::load(const std::string &name, const std::string &groupName,
	BWHardwareBuffer::Usage vertexBufferUsage /* = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY */, BWHardwareBuffer::Usage indexBufferUsage /* = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY */,
	bool vertexbufferShadowed /* = true */, bool indexBufferShadowed /* = true */)
{
	ResourceCreateOrRetrieveResult ret = CreateOrRetrieve(name, groupName, vertexBufferUsage, indexBufferUsage,
		vertexbufferShadowed, indexBufferShadowed);
	ret.first->Load();
	return ret.first;
}