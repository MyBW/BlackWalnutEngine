#ifndef MOVABLEOBJECT_H_
#define MOVABLEOBJECT_H_
#include "BWRenderable.h"
#include "BWRenderQueue.h"
#include "BWShadowCaster.h"
#include "BWSceneNode.h"
class BWMovableObjectFactory;
class BWMovableObject : public BWShadowCaster
{
public:
	BWMovableObject(const std::string &name , BWSceneManager *sceneManager);
	BWMovableObject(const std::string &name);
	~BWMovableObject();
	RenderQueueGroup getRenderQueueDes() const;
	void setSceneManager(BWSceneManager *sceneManager);
	BWSceneManager*  getSceneManager() const;
	void setCreator(BWMovableObjectFactory *creator);
	void setParentNode(BWNode *node);
	BWNode* getParentNode() const;
	BWSceneNode* getParentSceneNode();
	bool isVisible() ;
	void SetParentIsTagPoint(bool IsTagPoint) { mParentIsTagPoint = IsTagPoint; }
	virtual void updateRenderQueue(BWRenderQueue *renderQueue) = 0;
	const std::string& getName();
protected:
	virtual const BWMatrix4& _getParentNodeFullTransform() const;

	static int ID;
	int id;
	std::string mName;
	BWSceneManager *mSceneManager;

	BWNode *mParent;
	bool mInitialised;
	bool mParentIsTagPoint{ false }; // 父节点是否为标记节点  主要是用来表示动画的绑定等等
	RenderQueueGroup queueDes;
	BWMovableObjectFactory *mCreator;
};

class BWMovableObjectFactory
{
public:
	BWMovableObjectFactory();
	~BWMovableObjectFactory();
	virtual const std::string & getType() = 0;
	virtual BWMovableObject * createInstance(const std::string &objectName, BWSceneManager *sceneManager , 
		const std::string &resourceName , const std::string &groupName = "");	
protected:
	virtual BWMovableObject * createInstanceImp(const std::string &name , const std::string &resouceName,  const std::string &groupName) = 0 ;

};

#endif