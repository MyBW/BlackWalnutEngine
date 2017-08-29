#include "BWNode.h"

BWNode::BWNode() :mParent(NULL)
, mCacheTransformOutOfDate(false)
, mCacheTransform(BWMatrix4::IDENTITY)
, mScale(BWVector3D::UNIT_SCALE)
, mInheritOrientation(true)
, mInheritScale(true)
{

}
BWNode::~BWNode()
{

}
BWNode* BWNode::getParent() const
{
	return mParent;
}
void BWNode::setParent(BWNode *parent)
{
	mParent = parent;
	mParentNotified = false;
	needUpdate();
}
////////////////////////////////////////////方位相关
const BWQuaternion& BWNode::getOrientation() const
{
	return mOrientation;
}
void BWNode::setOrientation(const BWQuaternion& q) 
{
	mOrientation = q;
	mOrientation.normalize();
	needUpdate();
}
void BWNode::setOrientation(float w , float x , float y , float z)
{
	mOrientation.set(w, x, y, z);
	mOrientation.normalize();
	needUpdate();
}
void BWNode::resetOrientation()
{
	mOrientation = BWQuaternion::IDENTITY;
	needUpdate();
}
/////////////////////////////////////////////位置相关
void BWNode::setPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	needUpdate();
}
void BWNode::setPosition(const BWVector3D& pos)
{
	mPosition = pos;
	needUpdate();
}
const BWVector3D& BWNode::getPosition() const
{
	return mPosition;
}
/////////////////////////////////////////////缩放相关
void BWNode::setScale(const BWVector3D& scale)
{
	mScale = scale;
	needUpdate();
}
void BWNode::setScale(float x, float y, float z)
{
	mScale.x = x;
	mScale.y = y;
	mScale.z = z;
	needUpdate();
}
void BWNode::scale(const BWVector3D& scale)
{
	mScale = mScale * scale;
	needUpdate();
}
void BWNode::scale(float x, float y, float z)
{
	mScale.x *= x;
	mScale.y *= y;
	mScale.z *= z;
	needUpdate();
}
const BWVector3D& BWNode::getScale() const
{
	return mScale;
}
/////////////////////////////////////////////和父节点之间的关系
void BWNode::setInheritOrientation(bool inherit)
{
	mInheritOrientation = inherit;
	needUpdate();
}
bool BWNode::getInheritOrientation() const
{
	return mInheritOrientation;
}
void BWNode::setInheritScale(bool inherit) 
{
	mInheritScale = inherit;
	needUpdate();
}
bool BWNode::getInheritScale() const
{
	return mInheritScale;
}
///////////////////////////////////////////平移
void BWNode::translate(const BWVector3D& inTranslate, TransformSpace relative /* = TS_PARENT */)
{
	switch (relative)
	{
	case BWNode::TS_LOCAL:
		mPosition += mOrientation * inTranslate;
		break;
	case BWNode::TS_PARENT:
		mPosition += inTranslate;
		break;
	case BWNode::TS_WORLD:
		if (mParent)
		{
			mPosition += (mParent->getDerivedOrientation().Inverse() * inTranslate) / mParent->getDerivedScale();
		}
		else
		{
			mPosition += inTranslate;
		}
		break;
	default:
		break;
	}
	needUpdate();
}
void BWNode::translate(float x, float y, float z, TransformSpace relative /* = TS_PARENT */)
{
	BWVector3D trans;
	trans.x = x; trans.y = y; trans.z = z;
	translate(trans, relative);
}
void BWNode::translate(const BWMatrix4& axe, const BWVector3D& move, TransformSpace relative /* = TS_PARENT */)
{
	BWVector3D tmp = axe * move;
	translate(tmp, relative);
}
////////////////////////////////////////////旋转
void BWNode::roll(const Radian& angle, TransformSpace relative /* = TS_LOCAL */)
{
	assert(0);
}
void BWNode::yaw(const Radian& angle, TransformSpace relative /* = TS_LOCAL */)
{
	assert(0);
}
void BWNode::pitch(const Radian& angle, TransformSpace relative /* = TS_LOCAL */)
{
	assert(0);
}
void BWNode::rotate(const BWQuaternion& inRotate, TransformSpace relative /* = TS_LOCAL */)
{
	BWQuaternion qnorm = inRotate;
	qnorm.normalize();
	switch (relative)
	{
	case BWNode::TS_LOCAL:
		mOrientation = mOrientation * qnorm;
		break;
	case BWNode::TS_PARENT:
		mOrientation = qnorm * mOrientation;
		break;
	case BWNode::TS_WORLD:
		mOrientation = mOrientation * getDerivedOrientation().Inverse() * qnorm * getDerivedOrientation();
		break;
	default:
		break;
	}
	needUpdate();
}
void BWNode::rotate(const BWVector3D& axis, const Radian &angle, TransformSpace relateiv /* = TS_LOCAL */)
{
	BWQuaternion q;
	q.fromAngleAxis(angle, axis);
	rotate(q, relateiv);
	needUpdate();
}
BWMatrix4 BWNode::getLocalAxes() 
{
	BWVector3 axisX(1, 0, 0);
	BWVector3 axisY(0, 1, 0);
	BWVector3 axisZ(0, 0, 1);
	axisX = mOrientation * axisX;
	axisY = mOrientation * axisY;
	axisZ = mOrientation * axisZ;
	return BWMatrix4((float)axisX.x, (float)axisY.x, (float)axisZ.x, (float)0.0,
		             (float)axisX.y, (float)axisY.y, (float)axisZ.y, (float)0.0,
		             (float)axisX.z, (float)axisY.z, (float)axisZ.z, (float)0.0,
		             (float)0.0,     (float)0.0,     (float)0.0,     (float)0.0);
}
/////////////////////////////////////////相对于世界坐标系的设置
BWQuaternion BWNode::converLocalToWorldOrientation(const BWQuaternion &q)
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return mDerivedOrientation * q;
}
BWVector3D BWNode::converLocalToWorldPosition(const BWVector3D &localPos)
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return (mDerivedOrientation * localPos * mDerivedScale) + mDerivedPosition;
}
BWQuaternion BWNode::converWorldToLocationOrientation(const BWQuaternion &q)
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return mDerivedOrientation.Inverse() * q;
}
BWVector3D BWNode::converWorldToLocationPosition(const BWVector3D &worldPos)
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return mDerivedOrientation.Inverse() *(worldPos - mDerivedPosition) / mDerivedScale;
}

void BWNode::setDerivedOrientation(const BWQuaternion& q)
{
	setOrientation(mParent->converLocalToWorldOrientation(q));
}
void BWNode::setDerivedPosition(const BWVector3D& pos)
{
	setPosition(mParent->converWorldToLocationPosition(pos));
}
const BWQuaternion& BWNode::getDerivedOrientation() const
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return mDerivedOrientation;
}
const BWVector3D& BWNode::getDerivedPosition() const
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return mDerivedPosition;
}
const BWVector3& BWNode::getDerivedScale() const
{
	if (mNeedParentUpdate)
	{
		_updateFromParent();
	}
	return mDerivedScale;
}
const BWMatrix4& BWNode::getFullTransform()const
{
	if (mCacheTransformOutOfDate)
	{
		mCacheTransform.makeTransform(
			getDerivedPosition(),
			getDerivedScale(),
			getDerivedOrientation()
			);
		mCacheTransformOutOfDate = false;
	}
	return mCacheTransform;
}

////////////////////////////////////////父子关系
unsigned short BWNode::numChildren() const
{
	return mChildrenList.size();
}
BWNode* BWNode::createChild(const std::string& name, const BWVector3D& inTranslate /* = BWVector3D::ZERO */, const BWQuaternion& inRotate /* = BWQuaternion::IDENTITY */)
{
	return createChild(inTranslate, inRotate);
}
BWNode* BWNode::createChild(const BWVector3D& inTranslate /* = BWVector3D::ZERO */, const BWQuaternion& inRotate /* = BWQuaternion::IDENTITY */)
{
	BWNode* node = createChildImp();
	node->translate(inTranslate);
	node->rotate(inRotate);
	this->addChild(node);
	return node;
}
void BWNode::addChild(BWNode *node)
{
	if (node->getParent())
	{
		assert(0);
	}
	mChildrenList.push_back(node);
	node->setParent(this);
}
BWNode* BWNode::getChild(unsigned short index) const
{
	if( index < mChildrenList.size())
	{
		return mChildrenList[index];
	}
	assert(0);
}
BWNode* BWNode::getChild(const std::string& name) const
{
	assert(0);
	return NULL;
}
BWNode* BWNode::removeChild(unsigned short index)
{
	BWNode* tmp = NULL;
	if (index < mChildrenList.size())
	{
		tmp = mChildrenList[index];
		ChildrenList::iterator itor = mChildrenList.begin();
		while (index--)
		{
			itor++;
		}
		mChildrenList.erase(itor);
		cancelUpdate(tmp);
		tmp->setParent(NULL);
	}
	else
	{
		assert(0);
	}
	return tmp;
}
BWNode* BWNode::removeChild(BWNode *node)
{
	ChildrenList::iterator itor = mChildrenList.begin();
	ChildrenList::iterator itorEnd = mChildrenList.end();
	while (itorEnd != itor)
	{
		if (*itor == node)
		{
			mChildrenList.erase(itor);
			cancelUpdate(node);
			node->setParent(NULL);
			return node;
		}
		itor++;
	}
	return  NULL;
}
void BWNode::needUpdate()
{
	mNeedParentUpdate = true;
	mNeedChildrenUpdate = true;
	mCacheTransformOutOfDate = true;
	if (mParent && !mParentNotified)
	{
		mParent->requestUpdate(this);
		mParentNotified = true;
	}
	mChildrenUpdateVector.clear();
}

void BWNode::requestUpdate(BWNode *child)
{
	if (mNeedChildrenUpdate)
	{
		return;
	}
	mChildrenUpdateVector.push_back(child);
	if (mParent && !mParentNotified)
	{
		mParent->requestUpdate(this);
		mParentNotified = true;
	}
}
void BWNode::cancelUpdate(BWNode *child)
{
	ChildrenUpdateVector::iterator itor = mChildrenUpdateVector.begin();
	while (itor != mChildrenUpdateVector.end())
	{
		if (*itor == child)
		{
			mChildrenUpdateVector.erase(itor);
			if (mChildrenUpdateVector.size() == 0 && mParent && !mNeedChildrenUpdate)
			{
				mParent->cancelUpdate(this);
				mParentNotified = false;
			}
		}
		itor++;
	}
   if (itor == mChildrenUpdateVector.end())
   {
	   assert(0);
   }
}
void BWNode::update(bool updateChildren, bool parentHasChanged)
{
	mParentNotified = false;

    if (!updateChildren && !mNeedParentUpdate && !mNeedChildrenUpdate && !parentHasChanged)
    {
		return;
    }
	if (mNeedParentUpdate || parentHasChanged)
	{
		_updateFromParent();
	}
	if (mNeedParentUpdate || updateChildren)
	{
		ChildrenList::iterator itor, endItor;
		itor = mChildrenList.begin();
		endItor = mChildrenList.end();
		while (itor != endItor)
		{
			(*itor)->update(true, true);
			itor++;
		}
	}
	else
	{
		ChildrenUpdateVector::iterator itor = mChildrenUpdateVector.begin();
		ChildrenUpdateVector::iterator endItor = mChildrenUpdateVector.end();
		while (itor != endItor)
		{
			(*itor)->update(true, true);
			itor++;
		}
		mChildrenUpdateVector.clear();
	}
	mNeedChildrenUpdate = false;
}


void BWNode::getRenderOperation(BWRenderOperation &op) const
{

}


void BWNode::_updateFromParent() const
{
	updateFromParentImp();
}

void BWNode::updateFromParentImp() const
{
	if (mParent)
	{
		const BWQuaternion &parentOrientation = mParent->getDerivedOrientation();
		if (mInheritOrientation)
		{
			mDerivedOrientation = parentOrientation * mOrientation;
		}
		else
		{
			mDerivedOrientation = mOrientation;
		}

		const BWVector3D& parentScale = mParent->getDerivedScale();
		if (mInheritScale)
		{
			mDerivedScale = parentScale * mScale;
		}
		else
		{
			mDerivedScale = mScale;
		}

		mDerivedPosition = parentOrientation * (parentScale * mPosition);
		mDerivedPosition += mParent->getDerivedPosition();
	}
	else
	{
		mDerivedPosition = mPosition;
		mDerivedOrientation = mOrientation;
		mDerivedScale = mScale;
	}
	mCacheTransformOutOfDate = true;
	mNeedParentUpdate = false;
}


