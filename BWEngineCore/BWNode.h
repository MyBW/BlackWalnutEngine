#ifndef NODE_H_
#define NODE_H_
#include "BWRenderable.h"
#include <list>
class BWNode : public BWRenderable
{
public:
	enum TransformSpace
	{
		TS_LOCAL ,  // ��������������ϵ	
		TS_PARENT ,
		TS_WORLD
	};
	typedef std::vector<BWNode*> ChildrenUpdateVector;
	typedef std::vector<BWNode*> ChildrenList;

	BWNode();
	~BWNode();
	BWNode* getParent() const;
	void setParent(BWNode *parent);
	//��λ��ص� 
	virtual const BWQuaternion& getOrientation() const;
	virtual void setOrientation(const BWQuaternion& q);
	virtual void setOrientation(float w, float x, float y, float z);
	virtual void resetOrientation();
	//λ��
	virtual void setPosition(const BWVector3D& pos);
	virtual void setPosition(float x, float y, float z);
	virtual const BWVector3D& getPosition() const;
	//����
	virtual void setScale(const BWVector3D& scale); // ֱ�ӽ�ԭ����scale���ǵ�
	virtual void setScale(float x, float y, float z);// ֱ�ӽ�ԭ����scale���ǵ�
	virtual void scale(const BWVector3D& scale); // ��ԭ���Ļ�����ʹ�ø�scale
	virtual void scale(float x, float y, float z);// ��ԭ���Ļ�����ʹ�ø�scale
	virtual const BWVector3D& getScale() const;
	//�͸��ڵ�֮��Ĺ�ϵ
	virtual void setInheritOrientation(bool inherit);
	virtual bool getInheritOrientation() const;
	virtual void setInheritScale(bool inherit);
	virtual bool getInheritScale() const;
	//ƽ��
	virtual void translate(const BWVector3D& inTranslate, TransformSpace relative = TS_PARENT);
	virtual void translate(float x , float y , float z, TransformSpace relative = TS_PARENT);
	// ������ϵaxe��(������ʱʹ��4x4�������3x3����) ����move �ƶ��ڵ�  ��Ȼ��̫��ʲô��˼ ����������
	virtual void translate(const BWMatrix4& axe, const BWVector3D& move, TransformSpace relative = TS_PARENT);
	//��ת  ��������ڱ������� Ҳ�����ϴ���ת��Ľ��(����ϵҲһͬת��)
	virtual void roll(const Radian& angle, TransformSpace relative = TS_LOCAL);
	virtual void pitch(const Radian& angle, TransformSpace relative = TS_LOCAL);
	virtual void yaw(const Radian& angle, TransformSpace relative = TS_LOCAL);
	virtual	void rotate(const BWQuaternion& inRotate, TransformSpace relative = TS_LOCAL);
	virtual void rotate(const BWVector3D& axis, const Radian &angle, TransformSpace relateiv = TS_LOCAL);
	//�õ���ǰ�ڵ������ϵ������ڸ��ڵ㣩 
	virtual BWMatrix4 getLocalAxes() ;
	//����ɾ���ڵ�
    virtual BWNode* createChild(const BWVector3D& inTranslate = BWVector3D::ZERO, const BWQuaternion& inRotate = BWQuaternion::IDENTITY);
	virtual BWNode* createChild(const std::string& name, const BWVector3D& inTranslate = BWVector3D::ZERO, const BWQuaternion& inRotate = BWQuaternion::IDENTITY);
	virtual void addChild(BWNode *node);
	virtual unsigned short numChildren() const;
	virtual BWNode* getChild(unsigned short index) const;
	virtual BWNode* getChild(const std::string& name) const;
	virtual BWNode* removeChild(unsigned short index);
	virtual BWNode* removeChild(BWNode *node);

	// �õ��൱����������ϵ������
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
	// updateChildren: �Ƿ��������
	// parentHasChenged �� �����Ƿ�ı�
	void update(bool updateChildren, bool parentHasChanged);

	// �ڵ�Ҳ�ǿ���Ⱦ�� ��������debug
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
    
	mutable	BWMatrix4 mCacheTransform;  //���յõ��ľ���
	mutable ChildrenUpdateVector mChildrenUpdateVector;

	BWQuaternion mOrientation; // ��Ը��ڵ�ķ�λ

	BWVector3D mPosition;     //��Ը��ڵ��λ��

	BWVector3D mScale;

	bool mInheritOrientation; // �ýڵ��Ƿ�Ӹ��ڵ�̳еķ�λ
	bool mInheritScale;       // �ýڵ��Ƿ�Ӹ��ڵ�̳�����

	mutable BWQuaternion mDerivedOrientation;   //�������ķ�λ����λһ������˴Ӹ����ӵ�һ�б任��

	mutable BWVector3D mDerivedPosition;    //�����������ϵ��λ��

	mutable BWVector3D mDerivedScale;      // �����������ϵ������
};
#endif