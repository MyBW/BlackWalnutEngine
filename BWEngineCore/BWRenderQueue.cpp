#include "BWRenderQueue.h"
#include "BWMaterialManager.h"
#include "BWMovableObject.h"
#include"BWFrustum.h"
BWRenderQueueGroup::BWRenderQueueGroup()
{

}
BWRenderQueueGroup::~BWRenderQueueGroup()
{

}
void BWRenderQueue::processVisibleObject( BWMovableObject *mo, BWFrustum *camera, bool onlyShadowCaster, VisibleObjectsBoundsInfo *visibleBounds)
{
	bool receiveShadow = getQueueGroup(mo->getRenderQueueDes())->getShadowEnable() && mo->getReceiveShadows();

	if (mo->isVisible() && (!onlyShadowCaster || mo->getCastShadows()))
	{
	   mo->updateRenderQueue(this);
	   if (visibleBounds)
	   {
		  // assert(0);
		   //���¿��ӷ�Χ
	   }
	}
	else
	{
		//assert(0); 
		//��Ӱ��صĺ���
	}
}
void BWRenderQueue::addRenderable(const BWRenderable &renderable, unsigned short renderQueueID, unsigned short renderQueuePriority)
{
	RenderQueueGroupList::iterator itor = renderQueueGroupList.find(renderQueueID);
	BWRenderQueueGroup* group = NULL;
	if (itor == renderQueueGroupList.end())
	{
		group = new BWRenderQueueGroup();
		renderQueueGroupList.insert(RenderQueueGroupList::value_type(renderQueueID, group));
		itor = renderQueueGroupList.find(renderQueueID);
	}
	else
	{
		group = itor->second;
	}
	if (!renderable.getMaterial().IsNull())
	{
		renderable.getMaterial()->touch();
	}
	BWTechnique *tech;
	if (renderable.getMaterial().IsNull() || !renderable.getTechnology())
	{
		BWMaterialPtr basematerial = BWMaterialManager::GetInstance()->GetResource("BaseWhite");
		tech = basematerial->getTechnique(0);
	}
	else
	{
		tech = renderable.getTechnology();
	}
	group->addRenderable(renderable , tech, renderQueuePriority);
}
void BWRenderQueue::addRenderable(const BWRenderable &renderable)
{
	addRenderable(renderable, mDefaultRenderQueueID, mDdefaultRenderPriority);
}
void BWRenderQueue::addRenderable(const BWRenderable &renderable, unsigned short renderQueueID)
{
	addRenderable(renderable, renderQueueID, mDdefaultRenderPriority);
}

BWRenderQueue::BWRenderQueue()
{

}
BWRenderQueueGroup* BWRenderQueue::getQueueGroup(RenderQueueGroup renderQueueGroup)
{
	RenderQueueGroupList::iterator itor = renderQueueGroupList.find(renderQueueGroup);
	if (itor != renderQueueGroupList.end())
	{
		return itor->second;
	}
	return NULL;
}
RenderQueueGroupList& BWRenderQueue::getRenderQueueGroupList()
{
	return renderQueueGroupList;
}
void BWRenderQueue::clear()
{
	RenderQueueGroupList::iterator itor = renderQueueGroupList.begin();
	while (itor != renderQueueGroupList.end())
	{
		itor->second->clear();
		itor++;
	}
}
bool BWRenderQueueGroup::getShadowEnable()
{
	return false;
}
RenderPriorityGroupList&  BWRenderQueueGroup::getRenderPriorityGroupList()
{
	return renderPriorityGropList;
}
void BWRenderQueueGroup::clear()
{
	RenderPriorityGroupList::iterator itor = renderPriorityGropList.begin();
	while (itor != renderPriorityGropList.end())
	{
		itor->second->clear();
		itor++;
	}
}
void BWRenderQueueGroup::addRenderable(const BWRenderable &renderable , BWTechnique *tech , unsigned short renderPriority)
{
	//�������ݾ���hrenderPriority û������ ���Ǳȼ�����renderable��˭����Ⱦ
	  RenderPriorityGroupList::iterator itor = renderPriorityGropList.find(renderPriority);
	  BWRenderPriorityGroup* priorityGroup =  NULL;
	  if (itor == renderPriorityGropList.end())
	  {
		  priorityGroup = new BWRenderPriorityGroup();
		  renderPriorityGropList.insert(RenderPriorityGroupList::value_type(renderPriority, priorityGroup));
	  }
	  else
	  {
		  priorityGroup = itor->second;
	  }
	  priorityGroup->addRenderable(renderable , tech);
}

BWRenderPriorityGroup::BWRenderPriorityGroup()
{
}
BWRenderPriorityGroup::~BWRenderPriorityGroup()
{

}
void BWRenderPriorityGroup::clear()
{
	mCommonRenderableCollection.clear();
	mTransparentRenderableCollection.clear();
}
void BWRenderPriorityGroup::addRenderable(const BWRenderable &renderable , BWTechnique *tech)
{
	//�������renderable �Ƿ�͸�� ��Ȼ���������ɫ�������Ƿ��� ��������Ⱦ����
	//Ŀǰֻ�ж��Ƿ�͸��
	if (tech->isTransparent())
	{
		addTranparentCollection(renderable , tech);
		return;
	}
	addToCommomCollection(renderable , tech);
}
void BWRenderPriorityGroup::addTranparentCollection(const BWRenderable &renderable, BWTechnique *tech)
{
	const BWTechnique::PassList & passList = tech->getPassList();
	BWTechnique::PassList::const_iterator itor = passList.begin();
	while (itor != passList.end())
	{
		mTransparentRenderableCollection.addRenderable(*itor, renderable);
		itor++;
	}
}
void BWRenderPriorityGroup::addToCommomCollection(const BWRenderable &renderable, BWTechnique *tech)
{
	const BWTechnique::PassList & passList = tech->getPassList();
	BWTechnique::PassList::const_iterator itor = passList.begin();
	while(itor != passList.end())
	{
		mCommonRenderableCollection.addRenderable(*itor, renderable);
		itor++;
	}
}
BWQueueRenderableCollection& BWRenderPriorityGroup::getSolidsBasic()
{
	return mCommonRenderableCollection;
}
BWQueueRenderableCollection& BWRenderPriorityGroup::getTransparent()
{
	return mTransparentRenderableCollection;
}
BWQueueRenderableCollection::BWQueueRenderableCollection()
{
	mOrganisationMode = OM_PASS_GROUP;
}
BWQueueRenderableCollection::~BWQueueRenderableCollection()
{

}
void BWQueueRenderableCollection::clear()
{
	PassRenderableList::iterator itor = passRenderableList.begin();
	while (itor != passRenderableList.end())
	{
		itor->second->clear();
		delete (itor->second);
		itor++;
	}
	passRenderableList.clear();
}
void BWQueueRenderableCollection::addRenderable(const BWPass *pass , const BWRenderable &renderable)
{
	//���������ַ��෽ʽ һ���Ǹ���pass  �ڶ��ָ��ݵ��������Զ�� �ӽ���Զ  ������ ��Զ���� 
	//����ֻʵ�ֵ�һ��
	PassRenderableList::iterator itor = passRenderableList.find(pass);
	RenderableList * renderableList = NULL ;
	if (itor == passRenderableList.end())
	{
		renderableList = new RenderableList();
		passRenderableList.insert(PassRenderableList::value_type(pass , renderableList));
	}
	else
	{
		renderableList = itor->second;
	}
	renderableList->push_back(&renderable);
}

void BWQueueRenderableCollection::accept( BWQueueRenderableVisitor& visitor, OrganisationMode mo)
{
	if ((mo & mOrganisationMode) == 0)
	{
		if (OM_PASS_GROUP & mOrganisationMode)
		{
			mo = OM_PASS_GROUP;
		}
		else if (OM_SORT_ASCENDING & mOrganisationMode)
		{
			mo = OM_SORT_ASCENDING;
		}else if (OM_SORT_DESCENDING & mOrganisationMode)
		{
			mo = OM_SORT_DESCENDING;
		}
		else
		{
			assert(0);
		}
	}
	switch (mo)
	{
	case BWQueueRenderableCollection::OM_PASS_GROUP:
		acceptVisitorGrouped(visitor);
		break;
	case BWQueueRenderableCollection::OM_SORT_DESCENDING:
		acceptVisitorDescending(visitor);
		break;
	case BWQueueRenderableCollection::OM_SORT_ASCENDING:
		acceptVisitorAscending(visitor);
		break;
	default:
		break;
	}
}

void BWQueueRenderableCollection::acceptVisitorGrouped( BWQueueRenderableVisitor &visitor)
{
	PassRenderableList::iterator itor = passRenderableList.begin();
	PassRenderableList::iterator endItor = passRenderableList.end();
	while (itor != endItor)
	{
		if (itor->second->empty())
		{
			continue;
		}
		if (!visitor.visitor(itor->first)) // �ȷ���pass
		{
			continue;
		}
		RenderableList *renderableList = itor->second;
		RenderableList::iterator renderableItor = renderableList->begin();
		RenderableList::iterator endRenderableItor = renderableList->end();
		while (renderableItor != endRenderableItor) // �ڷ�λrenderable
		{
			visitor.visitor( const_cast<BWRenderable*>(*renderableItor));
			renderableItor++;
		}
		itor++;
	}
}
void BWQueueRenderableCollection::acceptVisitorAscending(BWQueueRenderableVisitor &visitor) 
{

}
void BWQueueRenderableCollection::acceptVisitorDescending(BWQueueRenderableVisitor &visitor)
{

}
void BWQueueRenderableCollection::setOrganisationMode(OrganisationMode model)
{
	mOrganisationMode = model;
}