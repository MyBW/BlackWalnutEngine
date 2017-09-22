#ifndef FRUSTUM_H_
#define FRUSTUM_H_
#include "BWMovableObject.h"
#include "BWRenderable.h"

enum ProjectionType
{
	PT_ORTHOGRAPHIC,
	PT_PROJECTIVE
};

class BWFrustum :  public BWMovableObject
{
public:
	BWFrustum(const std::string &name, BWSceneManager *sceneManager);
	BWFrustum();
	~BWFrustum(){ }
	bool isVisible(const AxisAlignedBox& bound);
	virtual bool isFrustumOutOfDate() const;
	virtual bool isViewMatrixOutOfDate() const;
	// ͶӰ����
	virtual const BWMatrix4& getProjectionMatrix() const;
	void updateFrustum() const;
	virtual const BWMatrix4& getPreProjectionMatrixRS() const;
	virtual const BWMatrix4& getProjectionMatrixRS() const;
	BWMatrix4 getProjectionMatrixWithRSDepth() const;
	
	float getOrthoWindowWidth() const;
	void setOrthoWindowHeight(float OrhtoHeight) { mOrhtoHeight = OrhtoHeight; }
	float getOrthoWindowHeight() const;
	void  SetAspect(float Aspect) { mAspect = Aspect; }
	// ��ͼ����
	BWMatrix4& getPreViewMatrix() const;
	BWMatrix4& getViewMatrix() const;
	void updateViewMatrix() const;
	
	void setFarClipDistance(float farPlane);
	void setNearClipDistance(float nearPlane);
	float getNearClipDistance() const;
	float getFarClipDistance() const;
	const Radian& getFOVy() const;
	float GetAspect() const { return mAspect; }
	void getWorldTransforms(BWMatrix4* worldmatrix) const;
	virtual const BWQuaternion& getOrientationForViewUpdate() const;
	virtual const BWVector3D& getPositionForViewUpdate() const;
	void calcViewMatrixRelative(const BWVector3 &relPos, BWMatrix4 &matToUpdata) const;
	void calcProjectionParameters(float &left, float &right, float &bottom, float &top) const;
	virtual void invalidateFrustum();
	virtual void invalidateView();
	void SetProjectType(ProjectionType ProjType) { mProjectionType = ProjType; }
	void EndFrame() const;
protected:
	virtual void updateFrustumImp() const;
	virtual void updateViewMatrixImp() const;

	// OpenGL ����ͶӰ���� Ҳ�������־���
    mutable	BWMatrix4 mProjectMatrix;
	//RenderSystem ת������ľ���
	mutable BWMatrix4 mProjectMatrixRS;
	mutable BWMatrix4 mPreProjectionMatrixRS;
	//�������Χ��
	mutable AxisAlignedBox mBoudingBox;
	//�������
	mutable bool mRecalcFrustum;
	mutable bool mRecalcFrustumPlanes;
	mutable bool mReCalcWorldSpeceCorners;
	mutable bool mReflect;
	// �Ƿ�ʹ���û��ṩ�ľ���
	bool mCustomProjMatrix;
	//�Ƿ�ʹ��бͶӰ����
	bool mObliqueDepthProjection;

	// �ֶ�������׶��
	bool mFrustumExtentsManuallySet;
	Radian mFOVy; 
	float mAspect; // ��߱�
	float mOrhtoHeight;
	float mFocalLength; // ����
	BWVector3D mFrustumOffset;

	mutable float mLeft;
	mutable float mRight;
	mutable float mTop;
	mutable float mBottom;
	float mFarDist;
	float mNearDist;
	ProjectionType mProjectionType;

	// ��ͼ����
	mutable BWMatrix4 mViewMatrix;
	mutable BWMatrix4 mPreViewMatrix;
	BWMatrix4 mReflectMatrix;              //����ʲô��˼
	mutable BWVector3D mLastParentPosition;   // Node��λ��
	mutable BWQuaternion mLastParentOrientation; // Node�ķ�λ
	mutable bool mRecalcView;
	static float INFINITE_FAR_PLANE_ADJUST;
};
#endif