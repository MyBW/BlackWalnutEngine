#ifndef CAMERA_H_
#define CAMERA_H_
#include "BWSceneManager.h"
#include "BWViewport.h"
#include "BWFrustum.h"
class BWCamera : public BWFrustum
{
	friend class BWSceneManager;
	friend class BWViewport;
public:
	BWCamera();
	BWCamera(BWSceneManager* creator, const std::string &name);
	~BWCamera();
	const BWMatrix4& getViewMatrix(bool ownFrustumOnly) const ;
	const BWMatrix4& getPreViewMatrix(bool ownFrustumOnly)const;
	const BWMatrix4& getViewMatrix() const;
	BWCamera* getLodCamera() const ;

    const BWVector3& getDerivedPosition() const;
	BWVector3 getDerivedDirection() const;
	const BWQuaternion& getDerivedOrientation() const;
	BWVector3 getDerivedRight() const;
	BWVector3 getDerivedUp() const;

	const BWQuaternion& getRealOrientation() const;
	void setRealOrientation(const BWQuaternion &q);
	const BWVector3D& getRealPosition() const;
	void setRealPosition(const BWVector3D& v);
	BWVector3D getRealUp() const;
	BWVector3D getRealRight() const;
	BWVector3D getRealDirection() const;

	bool isViewMatrixOutOfDate() const;

	void setPosition(float x, float y, float z);
	void setPosition(const BWVector3D& vec);
	const BWVector3D& getPosition() const;
	void move(const BWVector3D& vec);
	void moveRelative(const BWVector3D& vec);
	void setDirection(float x, float y, float z);
	void setDirection(const BWVector3D& vec);
	BWVector3D getDirection() const;
	BWVector3D getUp() const;
	BWVector3D getRight() const;
	void lookAt(const BWVector3D& targetPoint);
	void lookAt(float x, float y, float z);
	void rotate(const BWVector3D& axis, const Radian& angle);
	void rotate(const BWQuaternion&q);
	void setFixedYawAxis(bool useFixed, const BWVector3D& fiexAxis = BWVector3D::UNIT_Y);

	PolygonMode getPolygonMode();
	void getRenderOperation(BWRenderOperation &op) const;
	void updateRenderQueue(BWRenderQueue *renderQueue);
	bool getAutoAspectRatio() const;
	void setAspectRatio(float spectRatio);

	void invalidateFrustum();
	void invalidateView();
	const BWQuaternion& getOrientationForViewUpdate() const;
	const BWVector3D& getPositionForViewUpdate() const;
	BWMaterialPtr getMaterial() const { return NULL; }
	void getRenderOperation(BWRenderOperation &op){ }

	void BWCamera::pitch(const Radian& angle);
	void BWCamera::yaw(const Radian& angle);
	void BWCamera::roll(const Radian& angle);

	bool IsTest;
	BWMatrix4 TestProjectMatrix;
	BWMatrix4 TestViewMatrix;
    const BWMatrix4& getProjectionMatrixRS() const override;

private:
	void _renderScene(BWViewport *viewport, bool showOverlays);
	void _autoTrack();
	void _notifyRenderedFaces(unsigned int faceNUm);
	void _notifyRenderedBathces(unsigned int batches);
	BWSceneManager *mSceneManager;
	mutable bool mRecalcWindow;
	// 可以自定义一个frustum来进行操作
	BWFrustum* mCullFrustum;
	
	BWQuaternion mOrientation;
	BWVector3D mPosition;

	mutable BWQuaternion mDerivedOrientation;
	mutable BWVector3D mDerivedPosition;
	//相对于世界的方位和位置
	mutable BWQuaternion mRealOrientation;
	mutable BWVector3D mRealPosition;

	bool mYawFixed; // 是否绕固定轴旋转
	BWVector3D mYawFixedAxis;

	std::string mName;
	PolygonMode mPolygonMode;
	bool mIsAutoAspectRatio; // 是否自动适配视口宽高比
	float maspectRatio;



	
};


#endif