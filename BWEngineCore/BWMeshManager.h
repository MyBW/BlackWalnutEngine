#ifndef MESHMANAGER_H_
#define MESHMANAGER_H_
#include "BWResourceManager.h"
#include "BWHardwareBuffer.h"
#include "BWMesh.h"
class BWMeshManager : public BWResourceManager, BWSingleton<BWMeshManager>
{
public:
	static BWMeshManager* GetInstance();
	BWMeshManager();
	~BWMeshManager();

	ResourceCreateOrRetrieveResult CreateOrRetrieve(
		const std::string &name, const std::string &groupName,
		BWHardwareBuffer::Usage vertexBufferUsage = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY,
		BWHardwareBuffer::Usage indexBufferUsage = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY,
		bool vertexbufferShadowed = true, bool indexBufferShadowed = true);
	BWResourcePtr  CreateImp(const std::string &name, const std::string &groupName);
	BWMeshPrt load(const std::string &name, const std::string &groupName,
		BWHardwareBuffer::Usage vertexBufferUsage = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY,
		BWHardwareBuffer::Usage indexBufferUsage = BWHardwareBuffer::HBU_STATIC_WRITE_ONLY,
		bool vertexbufferShadowed = true, bool indexBufferShadowed = true);
protected:
private:
};

#endif