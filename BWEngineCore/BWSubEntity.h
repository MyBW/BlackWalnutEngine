#ifndef SUBENTITY_H_
#define SUBENTITY_H_
#include "BWRenderable.h"
#include "BWSubMesh.h"

class BWSubEntity : public BWRenderable
{
public:
	friend class BWEntity;
	friend class BWSceneManager;
	virtual void getRenderOperation(BWRenderOperation &op) const;
	BWMaterialPtr getMaterial() const;
	BWTechnique* getTechnology() const;
	VertexDataPrt getVertexDataForBinding() const;
	unsigned short getNumWorldTransforms();
	void getWorldTransforms(BWMatrix4* worldmatrix) const;
protected:
	BWSubEntity(BWEntity* parent, BWSubMesh* subMesh);
	void setMaterialName(const std::string &name , const std::string &groupName);
	void setMaterial(BWMaterialPtr material);
	BWEntity *mParent;
	BWSubMesh *mSubMesh;
	std::string mMaterialName;
	BWMaterialPtr mMaterial;
	bool mIsVisable;
private:
};

#endif