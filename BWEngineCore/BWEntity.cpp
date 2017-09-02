#include "BWEntity.h"
#include "BWMeshManager.h"


BWEntity::BWEntity(const std::string &name, const BWMeshPrt &mesh) :BWMovableObject(name)
, mMesh(mesh)
, mHardwareAnimation(false)
{
	_initialse();
}

void BWEntity::_initialse()
{
	if (!mMesh->isLoad())
	{
		mMesh->Load();
	}
	buildSubEntityVector(mMesh, &mSubEntityList);
	if (mMesh->isLodManual())
	{
		unsigned short numLod;
		numLod = unsigned short(mMesh->getNumLodLevel());
		for (int i = 0; i < numLod; i++)
		{
			assert(0);
		}
	}
	reevaluateVertexProcessing();
	if (mParent)
	{
		getParentSceneNode()->needUpdate();
	}
	mInitialised = true;
	mMeshStateCount = mMesh->getStateCount();
}
void BWEntity::buildSubEntityVector(BWMeshPrt &mesh, SubEntityVector *subEntityVector)
{
	unsigned short numSubMesh;
	BWSubMesh* tmpSubMesh;
	BWSubEntity * tmpSubEntity;
	numSubMesh = mesh->getNumSubMeshes();
	for (int i = 0; i < numSubMesh; i++)
	{
		tmpSubMesh = mMesh->getSubMesh(i);
		tmpSubEntity = new BWSubEntity(this, tmpSubMesh);
		if (tmpSubMesh->isMatInitialised())
		{
			tmpSubEntity->setMaterialName(tmpSubMesh->getMaterialName(), mesh->GetGroupName());
		}
		subEntityVector->push_back(tmpSubEntity);
	}
}
BWEntity::VertexDataBindChoice BWEntity::chooseVertexDataForBinding(bool isVertexAnimation) const
{
	if (hasSkeleton())
	{
		if (!mHardwareAnimation)
		{
			return BIND_SOFTWARE_SKELETAL;
		}
		else if (isVertexAnimation)
		{
			return BIND_HARDWARE_MORPH;
		}
		else
		{
			return BIND_ORIGINAL;
		}
	}
	else if (isVertexAnimation)
	{
		if (mHardwareAnimation)
		{
			return BIND_HARDWARE_MORPH;
		}
		else
		{
			return BIND_SOFTWARE_MORPH;
		}
	}
	else
	{
		return BIND_ORIGINAL;
	}
}
VertexData* BWEntity::getVertexDataForBinding()
{
	VertexDataBindChoice c = chooseVertexDataForBinding(mMesh->getShareVertexDataAnimationType() != VAT_NONE);
	switch (c)
	{
	case BWEntity::BIND_ORIGINAL:
		return mMesh->sharedVertexData.Get();
	case BWEntity::BIND_SOFTWARE_SKELETAL:
		assert(0);
	case BWEntity::BIND_SOFTWARE_MORPH:
		assert(0);
	case BWEntity::BIND_HARDWARE_MORPH:
		assert(0);
	}
	return mMesh->sharedVertexData.Get();
}
void BWEntity::updateRenderQueue(BWRenderQueue *renderQueue) 
{
	if (!mInitialised)
	{
		return ;
	}
	if (mMesh->getStateCount() != mMeshStateCount)
	{
		_initialse();
	}
	SubEntityVector::iterator itor = mSubEntityList.begin();
	SubEntityVector::iterator itorEnd = mSubEntityList.end();
	//载入lod

	while (itorEnd != itor)
	{
		if ((*itor)->mIsVisable)
		{
			if (mRenderQueuePrioritySet)
			{
				renderQueue->addRenderable(*(*itor), mRenderQueueID, mRenderQueuePriority);
			}
			else if (mRenderQueueIDSet)
			{
				renderQueue->addRenderable(*(*itor), mRenderQueueID);
			}
			else
			{
				renderQueue->addRenderable(*(*itor));
			}
		}
		itor++;
	}

	//是否选择显示骨骼
}

void BWEntity::reevaluateVertexProcessing()
{

}

std::string BWEntityFactory::FACTORY_TYPE_ENTITY = "Entity";
const std::string & BWEntityFactory::getType()
{
	return FACTORY_TYPE_ENTITY;
}
BWMovableObject * BWEntityFactory::createInstanceImp(const std::string &objectName, const std::string resourceName, const std::string &groupName)
{
	BWMeshPrt tmpMesh;
	std::string tmpGroupName = groupName;
	if (groupName == "")
	{
		tmpGroupName = DEFAULT_RESOURCE_GROUP;
	}
	tmpMesh = BWMeshManager::GetInstance()->Create(resourceName, groupName);
	if (tmpMesh.IsNull())
	{
		assert(0);
	}
	return new BWEntity(objectName , tmpMesh);
}

