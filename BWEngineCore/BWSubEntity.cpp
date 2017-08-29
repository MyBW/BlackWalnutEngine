#include "BWSubEntity.h"
#include "BWMaterialManager.h"
#include "BWEntity.h"

BWSubEntity::BWSubEntity(BWEntity* parent, BWSubMesh* subMesh):
mParent(parent)
, mSubMesh(subMesh)
, mIsVisable(true)
, mMaterialName("BaseWhite")
{
	mMaterial = BWMaterialManager::GetInstance()->GetResource(mMaterialName, subMesh->getParent()->GetGroupName());
}
void BWSubEntity::setMaterialName(const std::string &name , const std::string &groupName)
{
	BWMaterialPtr material = BWMaterialManager::GetInstance()->GetResource(name, groupName);
	if (material.IsNull())
	{
		assert(0);
	}
	setMaterial(material);
}
void BWSubEntity::setMaterial(BWMaterialPtr material)
{
	if (material.IsNull())
	{
		assert(0);
	}
	mMaterial = material;
	mMaterial->Load();
	mMaterialName = mMaterial->GetResourceName();
	mParent->reevaluateVertexProcessing();
}
void BWSubEntity::getRenderOperation(BWRenderOperation &op) const
{
	mSubMesh->_getRenderOperation(op);
	op.vertexData = getVertexDataForBinding();
}
BWMaterialPtr BWSubEntity::getMaterial() const 
{
	return mMaterial;
}
BWTechnique* BWSubEntity::getTechnology() const
{
	return getMaterial()->getTechnique(0);
}
unsigned short BWSubEntity::getNumWorldTransforms()
{
	return 1;
}
void BWSubEntity::getWorldTransforms(BWMatrix4* worldmatrix) const
{
	*worldmatrix = mParent->_getParentNodeFullTransform();
}
VertexDataPrt BWSubEntity::getVertexDataForBinding() const
{
	if (mSubMesh->mIsUseSharedVertex)
	{
		return mParent->getVertexDataForBinding();
	}
	else
	{
		BWEntity::VertexDataBindChoice c =
			mParent->chooseVertexDataForBinding(mSubMesh->getVertextAnimationType() != VAT_NONE);
		switch (c)
		{
		case BWEntity::BIND_ORIGINAL:
			return mSubMesh->mVertexData;
		case BWEntity::BIND_SOFTWARE_SKELETAL:
			assert(0);
		case BWEntity::BIND_SOFTWARE_MORPH:
			assert(0);
		case BWEntity::BIND_HARDWARE_MORPH:
			assert(0);
		default:
			break;
		}
	}
}