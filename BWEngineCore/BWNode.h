#ifndef NODE_H_
#define NODE_H_
#include "BWRenderable.h"
#include <list>
class BWNode : public BWRenderable
{
public:
	enum TransformSpace
	{
		TS_LOCAL ,  // 类似于物体坐标系	
		TS_PARENT ,
		TS_WORLD
	};
	typedef std::vector<BWNode*> ChildrenUpdateVector;
	typedef std::vector<BWNode*> ChildrenList;

	BWNode();
	~BWNode();
	BWNode* getParent() const;
	void setParent(BWNode *parent);
	//方位相关的 
	virtual const BWQuaternion& getOrientation() const;
	virtual void setOrientation(const BWQuaternion& q);
	virtual void setOrientation(float w, float x, float y, float z);
	virtual void resetOrientation();
	//位置
	virtual void setPosition(const BWVector3D& pos);
	virtual void setPosition(float x, float y, float z);
	virtual const BWVector3D& getPosition() const;
	//缩放
	virtual void setScale(const BWVector3D& scale); // 直接将原来的scale覆盖掉
	virtual void setScale(float x, float y, float z);// 直接将原来的scale覆盖掉
	virtual void scale(const BWVector3D& scale); // 在原来的基础上使用该scale
	virtual void scale(float x, float y, float z);// 在原来的基础上使用该scale
	virtual const BWVector3D& getScale() const;
	//和父节点之间的关系
	virtual void setInheritOrientation(bool inherit);
	virtual bool getInheritOrientation() const;
	virtual void setInheritScale(bool inherit);
	virtual bool getInheritScale() const;
	//平移
	virtual void translate(const BWVector3D& inTranslate, TransformSpace relative = TS_PARENT);
	virtual void translate(float x , float y , float z, TransformSpace relative = TS_PARENT);
	// 在坐标系axe下(这里暂时使用4x4矩阵代替3x3矩阵) 沿着move 移动节点  虽然不太懂什么意思 。。。。。
	virtual void translate(const BWMatrix4& axe, const BWVector3D& move, TransformSpace relative = TS_PARENT);
	//旋转  都是相对于本地坐标 也就是上次旋转后的结果(坐标系也一同转动)
	virtual void roll(const Radian& angle, TransformSpace relative = TS_LOCAL);
	virtual void pitch(const Radian& angle, TransformSpace relative = TS_LOCAL);
	virtual void yaw(const Radian& angle, TransformSpace relative = TS_LOCAL);
	virtual	void rotate(const BWQuaternion& inRotate, TransformSpace relative = TS_LOCAL);
	virtual void rotate(const BWVector3D& axis, const Radian &angle, TransformSpace relateiv = TS_LOCAL);
	//得到当前节点的坐标系（相对于父节点） 
	virtual BWMatrix4 getLocalAxes() ;
	//创建删除节点
    virtual BWNode* createChild(const BWVector3D& inTranslate = BWVector3D::ZERO, const BWQuaternion& inRotate = BWQuaternion::IDENTITY);
	virtual BWNode* createChild(const std::string& name, const BWVector3D& inTranslate = BWVector3D::ZERO, const BWQuaternion& inRotate = BWQuaternion::IDENTITY);
	virtual void addChild(BWNode *node);
	virtual unsigned short numChildren() const;
	virtual BWNode* getChild(unsigned short index) const;
	virtual BWNode* getChild(const std::string& name) const;
	virtual BWNode* removeChild(unsigned short index);
	virtual BWNode* removeChild(BWNode *node);

	// 得到相当于世界坐标系的数据
	virtual void setDerivedPosition(const BWVector3D& pos);
	virtual void setDerivedOrientation(const BWQuaternion& q);
	virtual const BWVector3D& getDerivedPosition() const;
	virtual const BWQuaternion& getDerivedOrientation() const;
	virtual const BWVector3D& getDerivedScale() const;
	virtual const BWMatrix4& getFullTransform() const;
	BWVector3D BWNode::converWorldToLocationPosition(const BWVector3D &worldPos);
	BWQuaternion BWNode::converWorldToLocationOrientation(const BWQuaternion &q);
	BWVector3D BWNode::converLocalToWorldPosition(const BWVector3D &localPos);
	BWQuaternion BWNode::converLocalToWorldOrientation(const BWQuaternion &q);

	void needUpdate();
	void requestUpdate(BWNode *child);
	void cancelUpdate(BWNode *child);
	// updateChildren: 是否更新子类
	// parentHasChenged ： 父类是否改变
	void update(bool updateChildren, bool parentHasChanged);

	// 节点也是可渲染的 可以用来debug
	virtual void getRenderOperation(BWRenderOperation &op) const;
	BWMaterialPtr getMaterial() const { return NULL; }
protected:
	virtual void _updateFromParent() const ;
	virtual void updateFromParentImp() const;

	virtual BWNode* createChildImp() = 0;
	virtual BWNode* createChildImp(const std::string &name) = 0;


	mutable ChildrenList mChildrenList;

private:
	mutable bool mNeedParentUpdate;
	mutable bool mNeedChildrenUpdate;
	mutable bool mCacheTransformOutOfDate;
	bool mParentNotified;
	BWNode *mParent;
    
	mutable	BWMatrix4 mCacheTransform;  //最终得到的矩阵
	mutable ChildrenUpdateVector mChildrenUpdateVector;

	BWQuaternion mOrientation; // 相对父节点的方位

	BWVector3D mPosition;     //相对父节点的位置

	BWVector3D mScale;

	bool mInheritOrientation; // 该节点是否从父节点继承的方位
	bool mInheritScale;       // 该节点是否从父节点继承缩放

	mutable BWQuaternion mDerivedOrientation;   //相对世界的方位（方位一般包含了从根到子的一切变换）

	mutable BWVector3D mDerivedPosition;    //相对世界坐标系的位置

	mutable BWVector3D mDerivedScale;      // 相对世界坐标系的缩放
};
#endif