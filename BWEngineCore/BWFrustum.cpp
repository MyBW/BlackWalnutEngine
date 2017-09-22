#include "BWFrustum.h"
#include "BWRoot.h"
#include "BWRenderSystem.h"
#include <math.h>
float BWFrustum::INFINITE_FAR_PLANE_ADJUST = 0.00001;
BWFrustum::BWFrustum(const std::string &name, BWSceneManager *sceneManager):BWMovableObject(name, sceneManager)
, mCustomProjMatrix(false)
, mObliqueDepthProjection(false)
, mProjectionType(PT_PROJECTIVE)
, mViewMatrix(BWMatrix4::IDENTITY)
, mFarDist(100000.0f)
, mNearDist(100.f)
, mRecalcFrustum(true)
, mFOVy(3.1415 / 2)
, mAspect(4.0 / 3)
, mFocalLength(1.0)
, mFrustumExtentsManuallySet(false)
, mReflect(false)
{

}
BWFrustum::BWFrustum() :BWMovableObject("Frustum")
, mCustomProjMatrix(false)
, mObliqueDepthProjection(false)
, mProjectionType(PT_PROJECTIVE)
, mViewMatrix(BWMatrix4::IDENTITY)
, mFarDist(100000.0f)
, mNearDist(100.f)
, mRecalcFrustum(true)
, mFOVy(3.1415 / 2)
, mAspect(4.0/3)
, mFocalLength(1.0)
, mFrustumExtentsManuallySet(false)
,mReflect(false)
{

}
void BWFrustum::getWorldTransforms(BWMatrix4* worldmatrix) const
{
	if (mParent)
	{
		*worldmatrix = mParent->getFullTransform();
	}
	else
	{
		*worldmatrix = BWMatrix4::IDENTITY;
	}
}
bool BWFrustum::isVisible(const AxisAlignedBox& bound)
{
	return true;
}

void BWFrustum::calcViewMatrixRelative(const BWVector3 &dircet, BWMatrix4 & viewMatrix) const 
{
	
}

BWMatrix4 BWFrustum::getProjectionMatrixWithRSDepth() const
{
	BWMatrix4 tmp;
	return tmp;
}

const BWMatrix4& BWFrustum::getProjectionMatrix() const
{
	updateFrustum();
	return mProjectMatrix;
}
const BWMatrix4& BWFrustum::getProjectionMatrixRS() const
{
	updateFrustum();
	return mProjectMatrixRS;
}
void BWFrustum::updateFrustum() const
{
    if (isFrustumOutOfDate())
    {
		updateFrustumImp();
    }
}

const BWMatrix4& BWFrustum::getPreProjectionMatrixRS() const
{
	return mPreProjectionMatrixRS;
}

float BWFrustum::getOrthoWindowHeight() const
{
	return mOrhtoHeight;
}
float BWFrustum::getOrthoWindowWidth() const
{
	return mOrhtoHeight * mAspect;
}
void BWFrustum::updateFrustumImp() const
{
	float left, right, bottom, top;
	calcProjectionParameters(left, right, bottom, top);
	if (!mCustomProjMatrix)
	{
		float inv_w = 1.0 / (right - left);
		float inv_h = 1.0 / (top - bottom);
		float inv_d = 1.0 / (mFarDist - mNearDist);
		if (mProjectionType == PT_PROJECTIVE)
		{
			float A = 2 * mNearDist * inv_w;
			float B = 2 * mNearDist * inv_h;
			float C = (right + left) * inv_w;
			float D = (top + bottom) * inv_h;
			float q, qn;
			if ( mFarDist == 0)
			{
				q = BWFrustum::INFINITE_FAR_PLANE_ADJUST- 1;
				qn = mNearDist *(BWFrustum::INFINITE_FAR_PLANE_ADJUST - 2);
			}
			else
			{
				q = -(mFarDist + mNearDist) * inv_d;
				qn = -2 * (mFarDist * mNearDist) * inv_d;
			}
			// NB: This creates 'uniform' perspective projection matrix,
			// which depth range [-1,1], right-handed rules
			//
			// [ A   0   C   0  ]
			// [ 0   B   D   0  ]
			// [ 0   0   q   qn ]
			// [ 0   0   -1  0  ]
			//
			// A = 2 * near / (right - left)
			// B = 2 * near / (top - bottom)
			// C = (right + left) / (right - left)
			// D = (top + bottom) / (top - bottom)
			// q = - (far + near) / (far - near)
			// qn = - 2 * (far * near) / (far - near)

			mProjectMatrix.SetZero();
			mProjectMatrix[0][0] = A;
			mProjectMatrix[0][2] = C;
			mProjectMatrix[1][1] = B;
			mProjectMatrix[1][2] = D;
			mProjectMatrix[2][2] = q;
			mProjectMatrix[2][3] = qn;
			mProjectMatrix[3][2] = -1;
			if (mObliqueDepthProjection)
			{
				//斜投影 暂时不使用 
				assert(0); 
			}
		}
		else if (mProjectionType == PT_ORTHOGRAPHIC)
		{
			float A = 2 * inv_w;
			float B = 2 * inv_h;
			float C = -(right + left) *inv_w;
			float D = -(top + bottom) * inv_h;
			float q, qn;
			if (mFarDist == 0)
			{
				q = -BWFrustum::INFINITE_FAR_PLANE_ADJUST / mNearDist;
				qn = -BWFrustum::INFINITE_FAR_PLANE_ADJUST - 1;
			}
			else
			{
				q = -2 * inv_d;
				qn = -(mFarDist + mNearDist) * inv_d;
			}
			// NB: This creates 'uniform' orthographic projection matrix,
			// which depth range [-1,1], right-handed rules
			//
			// [ A   0   0   C  ]
			// [ 0   B   0   D  ]
			// [ 0   0   q   qn ]
			// [ 0   0   0   1  ]
			//
			// A = 2 * / (right - left)
			// B = 2 * / (top - bottom)
			// C = - (right + left) / (right - left)
			// D = - (top + bottom) / (top - bottom)
			// q = - 2 / (far - near)
			// qn = - (far + near) / (far - near)
			mProjectMatrix.SetZero();
			mProjectMatrix[0][0] = A;
			mProjectMatrix[0][3] = C;
			mProjectMatrix[1][1] = B;
			mProjectMatrix[1][3] = D;
			mProjectMatrix[2][2] = q;
			mProjectMatrix[2][3] = qn;
			mProjectMatrix[3][3] = 1;
		}
	}
	BWRenderSystem * renderSystem = BWRoot::GetInstance()->getRenderSystem();
	renderSystem->_convertProjectionMatrix(mProjectMatrix, mProjectMatrixRS);
	renderSystem->_convertProjectionMatrix(mProjectMatrix, mProjectMatrixRS, true);

	// 设置视锥体包围盒
	float farDist = (mFarDist == 0) ? 10000 : mFarDist;
	BWVector3D min(left, bottom, -farDist);
	BWVector3D max(right, top, 0);
	if (mCustomProjMatrix)
	{
		BWVector3D tmp = min;
		min.makeFloor(max);
		max.makeCeil(tmp);
	}
	if (mProjectionType == PT_PROJECTIVE)
	{
		// 如果是投影矩阵 那么以远裁剪面对应的正方形为包围盒
		float radio = farDist / mNearDist;
		min.makeFloor(BWVector3D(left*radio, bottom *radio, -farDist));
		max.makeCeil(BWVector3D(right * radio, top * radio, 0.0));
	}
	mBoudingBox.setExtents(min, max);
	mRecalcFrustum = false;
	mRecalcFrustumPlanes = true;
}

void BWFrustum::calcProjectionParameters(float &left, float &right, float &bottom, float &top) const
{
	if (mCustomProjMatrix)
	{ 
		// 通过用户提供的矩阵反推
		BWMatrix4 invProj = mProjectMatrix.inverse();
		BWVector3D topLeft(-0.5, 0.5, 0.0);
		BWVector3D bottomRight(0.5, -0.5, 0.0);
		topLeft = invProj * topLeft;
		bottomRight = invProj * bottomRight;
		left = topLeft.x;
		top = topLeft.y;
		right = bottomRight.x;
		bottom = bottomRight.y;
	}
	else
	{
		if (mFrustumExtentsManuallySet)
		{
			left = mLeft;
			right = mRight;
			top = mTop;
			bottom = mBottom;
		}
		else if (mProjectionType == PT_PROJECTIVE)
		{
			Radian thetaY(mFOVy.mRad * 0.5f);
			float tanThetaY = tan(thetaY.mRad);
			float tanThetaX = tanThetaY * mAspect;
			float nearFocal = mNearDist / mFocalLength;
			float nearOffsetX = mFrustumOffset.x * nearFocal;
			float nearOffsetY = mFrustumOffset.y * nearFocal;
			float half_w = tanThetaX * mNearDist;
			float half_h = tanThetaY * mNearDist;
			left = -half_w + nearOffsetX;
			right = half_w + nearOffsetX;
			bottom = -half_h + nearOffsetY;
			top = half_h + nearOffsetY;

			mLeft = left;
			mRight = right;
			mTop = top;
			mBottom = bottom;
		}
		else
		{
			float half_w = getOrthoWindowWidth() * 0.5;
			float half_h = getOrthoWindowHeight() * 0.5;
			mLeft = left = -half_w;
			mRight = right = half_w;
			mBottom = bottom = -half_h;
			mTop = top = half_h;
		}
	}
}

BWMatrix4& BWFrustum::getPreViewMatrix() const
{
	return mPreViewMatrix;
}

BWMatrix4& BWFrustum::getViewMatrix() const
{
	updateViewMatrix();
	return mViewMatrix;
}
void BWFrustum::updateViewMatrix() const
{
	if (isViewMatrixOutOfDate())
	{
		updateViewMatrixImp();
	}
}
void BWFrustum::updateViewMatrixImp() const
{
	if (!mCustomProjMatrix)
	{
		BWMatrix4 rot;
		const BWQuaternion& orientation = getOrientationForViewUpdate();
		const BWVector3D& position = getPositionForViewUpdate();
		mViewMatrix = BWMatrix4::makeViewMatrix(position, orientation, mReflect ? &mReflectMatrix : 0);
	}
	mRecalcView = false;
	mRecalcFrustumPlanes = true;
	mReCalcWorldSpeceCorners = true;
	if (mObliqueDepthProjection)
	{
		mRecalcFrustum = true;
	}
}
const BWQuaternion& BWFrustum::getOrientationForViewUpdate() const
{
	return mLastParentOrientation;
}
const BWVector3D& BWFrustum::getPositionForViewUpdate() const
{
	return mLastParentPosition;
}
bool BWFrustum::isViewMatrixOutOfDate() const
{
	if (mParent)
	{
		if (mRecalcView || 
			mParent->getDerivedOrientation() != mLastParentOrientation ||
			mParent->getDerivedPosition() != mLastParentPosition)
		{
			mLastParentOrientation = mParent->getDerivedOrientation();
			mLastParentPosition = mParent->getDerivedPosition();
			mRecalcView = true;
		}
	}
	// 这里计算reflection
	return mRecalcView;
}
bool BWFrustum::isFrustumOutOfDate() const
{
	if (mObliqueDepthProjection)
	{
		if (isViewMatrixOutOfDate())
		{
			mRecalcFrustum = true;
		}
		// 反射相关、、
	}
	return mRecalcFrustum;
}
void BWFrustum::setFarClipDistance(float farPlane)
{
	mFarDist = farPlane;
	invalidateFrustum();
}
void BWFrustum::setNearClipDistance(float nearPlane)
{
	if (nearPlane <= 0)
	{
		assert(0);
	}
	mNearDist = nearPlane;
	invalidateFrustum();
}
void BWFrustum::invalidateFrustum()
{
	mRecalcFrustum = true;
	mRecalcFrustumPlanes = true;
	mReCalcWorldSpeceCorners = true;
}
void BWFrustum::invalidateView()
{
	mRecalcView = true;
	mRecalcFrustumPlanes = true;
	mReCalcWorldSpeceCorners = true;
}

void BWFrustum::EndFrame() const
{
	mPreViewMatrix = getViewMatrix();
	mPreProjectionMatrixRS = getProjectionMatrixRS();
}

float BWFrustum::getNearClipDistance() const
{
	return mNearDist;
}
float BWFrustum::getFarClipDistance() const
{
	return mFarDist;
}
const Radian& BWFrustum::getFOVy() const
{
	return mFOVy;
}

