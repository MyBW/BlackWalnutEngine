#include "BWCamera.h"

BWCamera::BWCamera() :mSceneManager(NULL),mYawFixed(false)
{
}
BWCamera::BWCamera(BWSceneManager* creator, const std::string &name) : 
mSceneManager(creator), 
mName(name),
mIsAutoAspectRatio(true),
maspectRatio(4/3.0),
mPolygonMode(PM_SOLID),
mYawFixed(false)
{
	IsTest = false;
	TestViewMatrix.SetZero();
	TestProjectMatrix.SetZero();

}
void BWCamera::_renderScene(BWViewport *viewport, bool showOverlays)
{
	// 渲染前相应消息函数
	mSceneManager->_renderScene(this, viewport, showOverlays);
	//渲染完成后相应消息函数
}
void BWCamera::_autoTrack()
{

}

void BWCamera::_notifyRenderedBathces(unsigned int batches)
{

}

void BWCamera::_notifyRenderedFaces(unsigned int faceNUm)
{

}
PolygonMode BWCamera::getPolygonMode()
{
	return mPolygonMode;
}
BWCamera* BWCamera::getLodCamera() const
{
	return NULL;
}

bool BWCamera::getAutoAspectRatio() const
{
	return mIsAutoAspectRatio;
}
void BWCamera::setAspectRatio(float aspectratio)
{
	maspectRatio = aspectratio;
}
void BWCamera::getRenderOperation(BWRenderOperation &op) const
{

}
void BWCamera::updateRenderQueue(BWRenderQueue *renderQueue) 
{

}
//void BWCamera::getWorldTransforms(BWMatrix4* worldmatrix) const
//{
//	*worldmatrix =  mParent->getFullTransform();
//}

void BWCamera::setFixedYawAxis(bool useFixed, const BWVector3D& fiexAxis /* = BWVector3D::UNIT_Y */)
{
	mYawFixed = useFixed;
	mYawFixedAxis = fiexAxis;
}
void BWCamera::yaw(const Radian& angle)
{
	BWVector3 yAxis;
	if (mYawFixed) // 如果想使用第一人称摄像机 那么就要是固定的Y轴 Y轴朝向为单位Y轴
	{
		// Rotate around fixed yaw axis
		yAxis = mYawFixedAxis;
	}
	else
	{
		// Rotate around local Y axis
		yAxis = mOrientation * BWVector3::UNIT_Y;
	}

	rotate(yAxis, angle);

	invalidateView();
}

void BWCamera::pitch(const Radian& angle)
{
	// Rotate around local X axis
	BWVector3 xAxis = mOrientation * BWVector3::UNIT_X;
	rotate(xAxis, angle);

	invalidateView();

}
void BWCamera::roll(const Radian& angle)
{
	BWVector3 zAxis = mOrientation * BWVector3::UNIT_Z;
	rotate(zAxis, angle);
	invalidateView();
}


const BWMatrix4& BWCamera::getViewMatrix() const
{
	if (mCullFrustum)
	{
		return mCullFrustum->getViewMatrix();
	}
	else
	{
		return BWFrustum::getViewMatrix();
	}
}
const BWMatrix4& BWCamera::getViewMatrix(bool ownFrustumOnly) const
{
	if (IsTest)
	{
		return TestViewMatrix;
	}
	if (ownFrustumOnly)
	{
		return  BWFrustum::getViewMatrix();
	}
	else
	{
		return getViewMatrix();
	}
}

void BWCamera::setRealOrientation(const BWQuaternion &q)
{
	mOrientation = q;
	mOrientation.normalize();
	invalidateView();
}
const BWVector3&  BWCamera::getDerivedPosition() const
{
	updateViewMatrix();
	return mDerivedPosition;
}
const BWQuaternion& BWCamera::getDerivedOrientation() const
{
	updateViewMatrix();
	return mDerivedOrientation;
}
BWVector3D BWCamera::getDerivedDirection() const
{
	// 一直指向-Z
	updateViewMatrix();
	return mDerivedOrientation * BWVector3D::NEGATIVE_UNIT_Z;
}
BWVector3D BWCamera::getDerivedUp() const
{
	updateViewMatrix();
	return mDerivedOrientation * BWVector3D::UNIT_Y;
}
BWVector3D BWCamera::getDerivedRight() const
{
	updateViewMatrix();
	return mDerivedOrientation * BWVector3D::UNIT_X;
}

const BWQuaternion& BWCamera::getRealOrientation() const
{
	updateViewMatrix();
	return mRealOrientation;
}
void BWCamera::setRealPosition(const BWVector3D& pos)
{
	mRealPosition = pos;
}
const BWVector3D& BWCamera::getRealPosition() const
{
	updateViewMatrix();
	return mRealPosition;
}
BWVector3D BWCamera::getRealDirection() const
{
	updateViewMatrix();
	return mRealOrientation * BWVector3D::NEGATIVE_UNIT_Z;
}
BWVector3D BWCamera::getRealUp() const
{
	updateViewMatrix();
	return mRealOrientation * BWVector3D::UNIT_Y;
}
BWVector3D BWCamera::getRealRight() const
{
	updateViewMatrix();
	return mRealOrientation * BWVector3D::UNIT_X;
}
void BWCamera::setPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	invalidateView();
}
void BWCamera::setPosition(const BWVector3D& vec)
{
	mPosition = vec;
	invalidateView();
}
const BWVector3D& BWCamera::getPosition() const
{
	return mPosition;
}
void BWCamera::move(const BWVector3D& vec)
{
	mPosition = mPosition + vec;
	invalidateView();
}
void BWCamera::moveRelative(const BWVector3D& vec)
{
	BWVector3D trans = mOrientation * vec;
	mPosition = mPosition + trans;
	invalidateView();
}
void BWCamera::setDirection(float x, float y, float z)
{
	setDirection(BWVector3D(x, y, z));
}
void BWCamera::setDirection(const BWVector3D& vec)
{
	if (vec == BWVector3D::ZERO)
	{
		return;
	}
	BWVector3D zAdjustVec = vec* (-1);
	zAdjustVec.normalize();
	BWQuaternion targetWorldOrientation;
	if (mYawFixed)
	{
		BWVector3D xVec = mYawFixedAxis.crossProduct(zAdjustVec);
		xVec.normalize();
		BWVector3D yVec = zAdjustVec.crossProduct(xVec);
		yVec.normalize();
		targetWorldOrientation.fromAxes(xVec, yVec, mYawFixedAxis);
	}
	else
	{
		BWVector3D axes[3];
		updateViewMatrix();
		mRealOrientation.ToAxes(axes);
		BWQuaternion rotQuat;
		// 方向是现在的方向的180度
		if ((axes[2]+ zAdjustVec).lenth() < 0.00005f)
		{
			rotQuat.fromAngleAxis(PI, axes[1]);
		}
		else
		{
			rotQuat = axes[2].getRotationTo(zAdjustVec);
		}
		targetWorldOrientation = rotQuat * mRealOrientation;
	}
	if (mParent)
	{
		mOrientation = mParent->getDerivedOrientation().Inverse() *targetWorldOrientation;
	}
	else
	{
		mOrientation = targetWorldOrientation;
	}
	invalidateView();
}
void BWCamera::lookAt(const BWVector3D& targetPoint)
{
	updateViewMatrix();
	this->setDirection(targetPoint - mRealPosition);
}
void BWCamera::lookAt(float x, float y, float z)
{
	lookAt(BWEle3DD(x, y, z));
}
void BWCamera::rotate(const BWVector3D& axis, const Radian& angle)
{
	BWQuaternion q;
	q.fromAngleAxis(angle, axis);
	rotate(q);
}
void BWCamera::rotate(const BWQuaternion &q)
{
	BWQuaternion qnorm = q;
	qnorm.normalize();
	mOrientation = qnorm * mOrientation;
	invalidateView();
}
bool BWCamera::isViewMatrixOutOfDate() const
{
	if (mParent)
	{
	    if (mRecalcView ||
	    	mParent->getDerivedOrientation() != mLastParentOrientation ||
	    	mParent->getDerivedPosition() != mLastParentPosition)
	    {
	    	mLastParentOrientation = mParent->getDerivedOrientation();
	    	mLastParentPosition = mParent->getDerivedPosition();
			mRealOrientation = mLastParentOrientation * mOrientation;
			mRealPosition = (mLastParentOrientation * mPosition) + mLastParentPosition;
	    	mRecalcView = true;
			mRecalcWindow = true;
	    }
	}
	else
	{
		mRealOrientation = mOrientation;
		mRealPosition = mPosition;
	}

	// 反射相关的。。。。
	return mRecalcView;
}
void BWCamera::invalidateFrustum()
{
	mRecalcWindow = true;
	BWFrustum::invalidateFrustum();
}
void BWCamera::invalidateView()
{
	mRecalcWindow = true;
	BWFrustum::invalidateView();
}
const BWQuaternion& BWCamera::getOrientationForViewUpdate() const
{
	return mRealOrientation;
}
const BWVector3D& BWCamera::getPositionForViewUpdate() const
{
	return mRealPosition;
}

const BWMatrix4& BWCamera::getProjectionMatrixRS() const
{
	if (IsTest)
	{
		return TestProjectMatrix;
	}
	return  BWFrustum::getProjectionMatrixRS();
}