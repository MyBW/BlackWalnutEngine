#ifndef SCENENODE_H_
#define SCENENODE_H_
#include "BWAxisAlignedBox.h"
#include "BWNode.h"


class BWMovableObject;
class BWSceneNode : public BWNode
{
public:
	friend class BWSceneManager;
	typedef std::map<std::string , BWMovableObject*> MovableObjectMap;
	BWSceneNode(BWSceneManager *sceneManager);
	~BWSceneNode();
	BWMaterialPtr getMaterial() const { return NULL; }
	BWSceneNode *createChildSceneNode(const BWVector3D& inTranslate , const BWQuaternion & inRotate);
	virtual void attachObject(BWMovableObject * object);
	virtual void getRenderOperation(BWRenderOperation &op) ;
	void detache(BWMovableObject* moveable);
	void getWorldTransforms(BWMatrix4* worldmatrix) const;
	
protected:
	BWNode* createChildImp();
	BWNode* createChildImp(const std::string &name);
	virtual void _findVisibleObject(BWFrustum *camera, BWRenderQueue *renderQueue, VisibleObjectsBoundsInfo *visibleBounds,
		bool includeChildren = true, bool displayNode = false, bool onlyshdowCasters = false);

private:
	void _addBoundingBoxToRenderQueue(BWRenderQueue *renderQueue);
	BWSceneManager *mCreator;
	bool mHideBoundingBox;
	MovableObjectMap mMovableObjectMap;
	AxisAlignedBox mWorldAABB;

	void _autoTrack();
};

#endif