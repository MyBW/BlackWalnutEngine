#include "BWSceneNode.h"
#include "BWMovableObject.h"
#include "BWCamera.h"

BWSceneNode::BWSceneNode(BWSceneManager *sceneManager):mHideBoundingBox(false)
{
	mCreator = sceneManager;
}
BWSceneNode* BWSceneNode::createChildSceneNode(const BWVector3D& inTranslate, const BWQuaternion & inRotate)
{
	return static_cast<BWSceneNode*>(this->createChild(inTranslate, inRotate));
}
BWNode* BWSceneNode::createChildImp()
{
	assert(mCreator);
	return mCreator->createSceneNode();
}
BWNode* BWSceneNode::createChildImp(const std::string &name)
{
	return createChildImp();
}
void BWSceneNode::getRenderOperation(BWRenderOperation &op) 
{

}
void BWSceneNode::getWorldTransforms(BWMatrix4* worldmatrix) const 
{
	*worldmatrix = getFullTransform();
}
void BWSceneNode::detache(BWMovableObject* moveable)
{
	assert(mMovableObjectMap.find(moveable->getName()) != mMovableObjectMap.end());
	mMovableObjectMap.erase(mMovableObjectMap.find(moveable->getName()));
}
void BWSceneNode::attachObject(BWMovableObject * object)
{
	assert(mMovableObjectMap.find(object->getName()) == mMovableObjectMap.end());
	mMovableObjectMap[object->getName()] = object;
	object->setParentNode(this);
	
}
void BWSceneNode::_findVisibleObject(BWFrustum *camera, BWRenderQueue *renderQueue, VisibleObjectsBoundsInfo *visibleBounds,
	bool includeChildren /* = true */, bool displayNode /* = false */, bool onlyshdowCasters /* = false */)
{
	if (!camera->isVisible(mWorldAABB))
	{
		return;
	}
	MovableObjectMap::iterator itor = mMovableObjectMap.begin();
	MovableObjectMap::iterator itorEnd = mMovableObjectMap.end();
	while (itorEnd != itor)
	{
		renderQueue->processVisibleObject(itor->second, camera, onlyshdowCasters, visibleBounds);
		itor++;
	}
	if (includeChildren)
	{
		ChildrenList::iterator  itor = mChildrenList.begin();
		while (itor != mChildrenList.end())
		{
			BWSceneNode *child = static_cast<BWSceneNode*>(*itor);
			child->_findVisibleObject(camera, renderQueue, visibleBounds, includeChildren, displayNode, onlyshdowCasters);
			itor++;
		}
	}
	if (displayNode)
	{
		assert(0);
		//这里将节点放入渲染队列中
	}
	if (!mHideBoundingBox || (mCreator && mCreator->getShowBoundingBox()))
	{
		_addBoundingBoxToRenderQueue(renderQueue);
	}
}

void BWSceneNode::_addBoundingBoxToRenderQueue(BWRenderQueue *renderQueue)
{

}
void BWSceneNode::_autoTrack()
{

}