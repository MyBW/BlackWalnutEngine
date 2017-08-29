#ifndef ENTITY_H_
#define ENTITY_H_
#include "BWMovableObject.h"
#include "BWMesh.h"
#include "BWSubEntity.h"
#include "BWNode.h"

class BWEntity : public BWMovableObject
{
public:
	friend class BWEntityFactory;
	friend class BWSubEntity;
	enum VertexDataBindChoice
	{
		BIND_ORIGINAL ,
		BIND_SOFTWARE_SKELETAL ,
		BIND_SOFTWARE_MORPH ,
		BIND_HARDWARE_MORPH
	};
	~BWEntity(){ }
	BWEntity(const std::string &name, const BWMeshPrt &mesh);  // 原来是保护成员函数 暂时这样使用
	void updateRenderQueue(BWRenderQueue *renderQueue);
	BWSubEntity* GetSubEntity(int index) 
	{
		if (index >-1 && index < mSubEntityList.size())
		{
			return mSubEntityList[index];
		}
		return nullptr;
	}
protected:
	typedef  std::vector<BWSubEntity*> SubEntityVector;

	BWEntity ();
	void _initialse();
	void buildSubEntityVector(BWMeshPrt &mesh, SubEntityVector * subEntityVector);
	void reevaluateVertexProcessing();
	bool hasSkeleton() const { return false; }
	
	VertexData* getVertexDataForBinding();
	VertexDataBindChoice chooseVertexDataForBinding(bool vertexAnimation) const;
	BWMeshPrt mMesh;

	bool mRenderQueuePrioritySet;        // 该entity是否有优先级
	bool mRenderQueueIDSet;             // 该entity是否指定了放入的渲染队列（renderGroup）
	unsigned short mRenderQueueID;       // 指定的渲染队列的ID
	unsigned short mRenderQueuePriority; //指定的优先级
	unsigned short mMeshStateCount;      // 记录该mesh是否被重新载入
	SubEntityVector mSubEntityList;
	bool mHardwareAnimation;
};

class BWEntityFactory : public BWMovableObjectFactory
{
public:
	const std::string & getType();
protected:
	virtual BWMovableObject* createInstanceImp(const std::string &objectName, const std::string resourceName, const std::string &groupName);
private:
	static std::string FACTORY_TYPE_ENTITY;
};
#endif