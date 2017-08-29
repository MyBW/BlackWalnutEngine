#include "BWMovableObject.h"
int BWMovableObject::ID = 0;

BWMovableObject::BWMovableObject(const std::string &name , BWSceneManager *sceneManager) :queueDes(MAIN_RENDERQUEUEGROUP), id(ID)
, mName(name)
, mSceneManager(sceneManager)
,mParent(NULL)
{
	ID++;
	mParentIsTagPoint = false;
}
BWMovableObject::BWMovableObject(const std::string &name) :
queueDes(MAIN_RENDERQUEUEGROUP)
, id(ID)
, mName(name)
, mSceneManager(NULL)
,mParent(NULL)
{
	ID++;
	mParentIsTagPoint = false;
}
BWMovableObject::~BWMovableObject()
{

}
const std::string& BWMovableObject::getName() 
{
	return mName;
}
void BWMovableObject::setSceneManager(BWSceneManager *sceneManager)
{
	mSceneManager = sceneManager;
}
BWSceneManager*  BWMovableObject::getSceneManager() const
{
	return mSceneManager;
}
void BWMovableObject::setCreator(BWMovableObjectFactory *creator)
{
	mCreator = creator;
}
void BWMovableObject::setParentNode(BWNode *node)
{
	if (mParent)
	{
		 dynamic_cast<BWSceneNode*>(mParent)->detache(this);
	}
	mParent = node;
}
BWNode* BWMovableObject::getParentNode() const
{
	return mParent;
}
BWSceneNode* BWMovableObject::getParentSceneNode()
{
	if (mParentIsTagPoint)
	{
		// 这里是动画相关的
		assert(0);
	}
	else
	{
		return static_cast<BWSceneNode*>(mParent);
	}
}

bool BWMovableObject::isVisible()
{
	return true;
}


RenderQueueGroup BWMovableObject::getRenderQueueDes() const
{
	return queueDes;
}
const BWMatrix4& BWMovableObject::_getParentNodeFullTransform() const
{
	if (mParent)
	{
		return mParent->getFullTransform();
	}
}
BWMovableObject * BWMovableObjectFactory::createInstance(const std::string &name, BWSceneManager *sceneManager ,
	const std::string &resourceName, const std::string &groupName )
{
	BWMovableObject * m = createInstanceImp(name , resourceName , groupName);
	m->setSceneManager(sceneManager);
	m->setCreator(this);
	return m;
}

