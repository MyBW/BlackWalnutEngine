#ifndef RENDERQUEUE_H_
#define RENDERQUEUE_H_

#include "BWTechnique.h"
#include<map>
#include <vector>
class BWRenderable;
class BWMovableObject;
class BWCamera;
struct VisibleObjectsBoundsInfo;
enum RenderQueueGroup
{
	SKYBOX_RENDERQUEUEGROUP,
	MAIN_RENDERQUEUEGROUP,
	OVERLAY_RENDERQUEUEGROUP
};

class BWRenderQueueGroup;
class BWRenderPriorityGroup;
class BWQueueRenderableCollection;
class BWFrustum;
typedef std::map<unsigned short, BWRenderQueueGroup*>  RenderQueueGroupList;
typedef std::map<unsigned int, BWRenderPriorityGroup*> RenderPriorityGroupList;


struct RenderablePass
{
	BWRenderable *renderable;
	BWPass *pass;
	RenderablePass(BWRenderable *render, BWPass *p) :renderable(render), pass(p)
	{

	}
};

class BWQueueRenderableVisitor
{
public:
	BWQueueRenderableVisitor(){ }
	virtual ~BWQueueRenderableVisitor(){ }
	virtual void visitor(RenderablePass *renderablePass) = 0;
	virtual void visitor( BWRenderable *renderable) = 0;
	virtual bool visitor( const BWPass *pass) = 0;
};
class BWQueueRenderableCollection
{
public:
	typedef std::vector<const BWRenderable*> RenderableList;
	typedef std::map<const BWPass*, RenderableList*> PassRenderableList;
	enum OrganisationMode
	{
		OM_PASS_GROUP =1 ,  // 使用pass排序渲染
		OM_SORT_DESCENDING = 2, //使用和摄像机距离升序排序
		OM_SORT_ASCENDING =6    //使用和摄像机距离降序排序
	};
	BWQueueRenderableCollection();
	~BWQueueRenderableCollection();
	void clear();
	void addRenderable(const BWPass* pass, const BWRenderable& movable);
	void accept( BWQueueRenderableVisitor& visitor , OrganisationMode mo);
	void acceptVisitorGrouped( BWQueueRenderableVisitor &visitor);
	void acceptVisitorDescending( BWQueueRenderableVisitor &visitor);
	void acceptVisitorAscending( BWQueueRenderableVisitor &visitor);
	void setOrganisationMode(OrganisationMode model);
	PassRenderableList& GetPassRenderableList() { return passRenderableList; }
private:
	
	OrganisationMode mOrganisationMode;
	PassRenderableList passRenderableList;
};

class BWRenderPriorityGroup
{
public:
	BWRenderPriorityGroup();
	~BWRenderPriorityGroup();
	void addRenderable(const BWRenderable &renaderable , BWTechnique *tech);
	BWQueueRenderableCollection& getTransparent(); //  得到透明的渲染队列
	BWQueueRenderableCollection& getSolidsBasic(); //  得到非透明渲染队列
	void clear();
private:
	void addToCommomCollection(const BWRenderable &renderable , BWTechnique *tech);
	void addTranparentCollection(const BWRenderable &renderable , BWTechnique *tech);
	BWQueueRenderableCollection mCommonRenderableCollection;
	BWQueueRenderableCollection mTransparentRenderableCollection;
};

class BWRenderQueueGroup
{
public:
	BWRenderQueueGroup();
	~BWRenderQueueGroup();
	void addRenderable(const BWRenderable &renderable, BWTechnique* tech, unsigned short renderPriority);
	RenderPriorityGroupList& getRenderPriorityGroupList();
	bool getShadowEnable();
	void clear();
private:
	RenderPriorityGroupList renderPriorityGropList;
};


class BWRenderQueue
{
public:
	BWRenderQueue();
	~BWRenderQueue();
	BWRenderQueueGroup* getQueueGroup(RenderQueueGroup renderQueueGroup);
	RenderQueueGroupList& getRenderQueueGroupList();
	void clear();
	void addRenderable(const BWRenderable &movableObject);
	void addRenderable(const BWRenderable &movable, unsigned short  renderQueueID);
	void addRenderable(const BWRenderable &renderable, unsigned short renderQueueID, unsigned short renderQueuePriority);
	void processVisibleObject(BWMovableObject *mo, BWFrustum *camera, bool onlyShadowCaster, VisibleObjectsBoundsInfo *visibleBounds);
private:
	RenderQueueGroupList renderQueueGroupList;
	unsigned short mDefaultRenderQueueID;
	unsigned short mDdefaultRenderPriority;
};

#endif