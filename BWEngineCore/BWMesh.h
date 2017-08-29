#ifndef MESH_H_
#define MESH_H_
#include "BWResource.h"
#include "BWSubMesh.h"
#include "BWMeshSerializerImp.h"
#include "BWDataStream.h"
#include "BWCommon.h"
class BWMesh : public BWResource
{
	 friend class BWMeshSerializerImp;
	 friend class BWEntity;
	 friend class BWSubMesh;
public:
	typedef std::vector<BWSubMesh*> SubMeshList;
	typedef std::map<std::string, unsigned int> NamedSubMesh;
	BWMesh(BWResourceManager* creator, const std::string &name, const std::string &groupName);
	void setVertexBufferUsage(BWHardwareBuffer::Usage vertexBufferUsage , bool vertextBufferShadowed);
	void setIndexBufferUsage(BWHardwareBuffer::Usage indexBufferUsage , bool indexBufferShadowed);
	BWSubMesh * createSubMesh();
	void updateMaterialForAllSubMesh();
	unsigned short getNumSubMeshes();
	BWSubMesh * getSubMesh(unsigned int index);
	void nameSubMesh(const std::string &name, unsigned short index);
	bool isLodManual() { return mIsLodManual; }
	size_t getNumLodLevel(){ return mNumLods; }
	size_t calculateSize(){ return 0; }
	VertexAnimationType getShareVertexDataAnimationType(){ return VAT_NONE; }
	VertexDataPrt GetVertexData() { return sharedVertexData; }
protected:
	void preLoadImpl(){}
	void postLoadImpl(){}
	void loadImpl();
	void prepareImpl();
private:
	BWHardwareBuffer::Usage mIndexBufferUsage;
	BWHardwareBuffer::Usage mVertextBufferUsage;
	SubMeshList mSubMeshList;
	NamedSubMesh mSubMeshNamedMap;
	bool mVertexBufferShadowed;
	bool mIndexBufferShadowed;
	bool mAutoBuildEdgeLists;
	bool mIsLodManual; // 是否从文件读取lod信息
	unsigned short mNumLods; // 一共有多少lod层级
	//VertexData *sharedVertexData;
	VertexDataPrt sharedVertexData;
	BWDataStreamPrt mFreshFromDisk;
};

class BWMeshPrt : public SmartPointer<BWMesh>
{
public:
	BWMeshPrt() 
	{

	}
	BWMeshPrt(const BWResourcePtr &res)
	{
		if (mPointer == dynamic_cast<BWMesh*>(res.Get()))
		{
			return;
		}
		if (res.GetCounterPointer())
		{
			mPointer = dynamic_cast<BWMesh*>(res.Get());
			counter = res.GetCounterPointer();
			(*counter)++;
		}
	}
	const BWMeshPrt& operator=(const BWResourcePtr & res)
	{
		if (mPointer == dynamic_cast<BWMesh*>(res.Get()))
		{
			return *this;
		}
		if (mPointer != NULL)
		{
			(*counter)--;
			if ((*counter) == 0)
			{
				delete mPointer;
			}
		}
		mPointer = dynamic_cast<BWMesh*>(res.Get());
		counter = res.GetCounterPointer();
		if (counter != NULL)
		{
			(*counter)++;
		}
		return *this;
	}
protected:
private:
};
#endif