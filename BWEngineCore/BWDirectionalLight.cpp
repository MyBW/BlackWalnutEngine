#include"BWDirectionalLight.h"
#include"BWSceneManager.h"
#include"BWCamera.h"

float MMin(float a, float b) { return a > b ? b : a; }
float MMax(float a, float b) { return a > b ? a : b; }

BWDirectionalLight::BWDirectionalLight(const std::string &name, BWSceneManager* SceneManager) :BWLight(name , SceneManager , LT_DIRECTIONAL)
,CastShadowMapNum(3)
{
	mSceneManager->AddDirectLight(this);
	SetProjectType(ProjectionType::PT_ORTHOGRAPHIC);
}


void BWDirectionalLight::SetDirection(const BWVector3D& Direction)
{
	
	if (Direction == BWVector3D::ZERO)
	{
		return;
	}
	LightDirection = Direction;
	BWVector3D zAdjustVec = Direction* (-1);
	zAdjustVec.normalize();
	BWQuaternion targetWorldOrientation;
	
	
	
	BWVector3D axes[3];
	updateViewMatrix();
	mRealOrientation.ToAxes(axes);
	BWQuaternion rotQuat;
	// 方向是现在的方向的180度
	if ((axes[2] + zAdjustVec).lenth() < 0.00005f)
	{
		rotQuat.fromAngleAxis(PI, axes[1]);
	}
	else
	{
		rotQuat = axes[2].getRotationTo(zAdjustVec);
	}
	targetWorldOrientation = rotQuat * mRealOrientation;
	
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
bool BWDirectionalLight::isViewMatrixOutOfDate() const
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
	return mRecalcView;
}
void BWDirectionalLight::SetPosition(const BWVector3D& Position)
{
	mPosition = Position;
	invalidateView();
}
const BWQuaternion& BWDirectionalLight::getOrientationForViewUpdate() const
{
	return mRealOrientation;
}
const BWVector3D& BWDirectionalLight::getPositionForViewUpdate() const
{
	return mRealPosition;
}


bool BWDirectionalLight::SetCastShadowMapNum(int ShadowMapNum)
{
	if ( ShadowMapNum < 0|| ShadowMapNum > MAX_CAST_SHADOW_MAP_NUM)
	{
		return false;
	}
	CastShadowMapNum = ShadowMapNum;
	return true;
}
void BWDirectionalLight::InitProjectedShadowMap(int MapIndex, BWHardwareDepthBufferPtr NewShadowMap , bool IsWitheStencil)
{
	if (MapIndex < 0 || MapIndex > CastShadowMapNum)
	{
		return;
	}
	NewShadowMap->SetIsWithStencil(IsWitheStencil);
	ShadowInfo[MapIndex].ShadowTexture = NewShadowMap;
	ShadowInfo[MapIndex].ViewMatrix = getViewMatrix();
}
void BWDirectionalLight::InitProjectedShadowMap()
{
	for (int i = 0; i < CastShadowMapNum; i++)
	{
		assert(0);	
	}
}
ShadowMapProjectInfo& BWDirectionalLight::GetShadowMapProjectedInfor(int ShadowMapIndex)
{
	if (ShadowMapIndex < 0 || ShadowMapIndex > 2)
	{
		assert(0);
	}
	return ShadowInfo[ShadowMapIndex];
}
void BWDirectionalLight::UpdateProjectedShadowMap(const BWCamera *Camera)
{
	if (Camera)
	{
		BWMatrix4 ViewMatrix = Camera->getViewMatrix(true);
		BWMatrix4 LightViewMatrix = getViewMatrix();
		BWMatrix4 CameraMatrixInv = ViewMatrix.inverse();
		float NearPlane = Camera->getNearClipDistance();
		float FarPlane = Camera->getFarClipDistance();
		Radian FOV = Camera->getFOVy();
		float radio = Camera->GetAspect();
		float FrustumEnd[4];

		FrustumEnd[0] = NearPlane;
		FrustumEndWorldSpace[0].x = FrustumEndWorldSpace[0].y = 0;  FrustumEndWorldSpace[0].z = -FrustumEnd[0];  FrustumEndWorldSpace[0].w = 1.0;
		FrustumEnd[1] = (FarPlane - NearPlane) / 3 + NearPlane;
		FrustumEndWorldSpace[1].x = FrustumEndWorldSpace[1].y = 0;  FrustumEndWorldSpace[1].z = -FrustumEnd[1];  FrustumEndWorldSpace[1].w = 1.0;
		FrustumEnd[2] = (FarPlane - NearPlane) / 3 * 2 + NearPlane;
		FrustumEndWorldSpace[2].x = FrustumEndWorldSpace[2].y = 0;  FrustumEndWorldSpace[2].z = -FrustumEnd[2];  FrustumEndWorldSpace[2].w = 1.0;
		FrustumEnd[3] = FarPlane;
		FrustumEndWorldSpace[3].x = FrustumEndWorldSpace[3].y = 0;  FrustumEndWorldSpace[3].z = -FrustumEnd[3];  FrustumEndWorldSpace[3].w = 1.0;

		float TanFovDev2 = tan(FOV.mRad / 2.0);
		BWVector4 SubFrustum[16];
		for (int i = 0; i < 4; i++)
		{
			float X = TanFovDev2* FrustumEnd[i];
			float Y = X / radio;
			SubFrustum[i].x = X; SubFrustum[i].y = Y; SubFrustum[i].z = -FrustumEnd[i];  SubFrustum[i].w = 1.0;
			SubFrustum[i + 1].x = -X; SubFrustum[i + 1].y = Y; SubFrustum[i + 1].z = -FrustumEnd[i];  SubFrustum[i + 1].w = 1.0;
			SubFrustum[i + 2].x = -X; SubFrustum[i + 2].y = -Y; SubFrustum[i + 3].z = -FrustumEnd[i];  SubFrustum[i + 2].w = 1.0;
			SubFrustum[i + 3].x = X; SubFrustum[i + 3].y = -Y; SubFrustum[i + 3].z = -FrustumEnd[i];  SubFrustum[i + 3].w = 1.0;

			FrustumEndWorldSpace[i] = CameraMatrixInv * FrustumEndWorldSpace[i];
		}
		BWVector4 SubFrustumLightSpace[16];
		float MinX = 0;
		float MaxX = 0;
		float MinY = 0;
		float MaxY = 0;
		float MinZ = 0;
		float MaxZ = 0;
		for (int i = 0; i < 16; i++)
		{
			SubFrustumLightSpace[i] = LightViewMatrix * CameraMatrixInv * SubFrustum[i];
		}
		for (int i = 0; i < 3; i++)
		{
			MaxX = MinX = SubFrustumLightSpace[i * 4].x;
			MaxY = MinY = SubFrustumLightSpace[i * 4].y;
			MaxZ = MinZ = SubFrustumLightSpace[i * 4].z;
			for (int j = 0; j < 8; j++)
			{
				MinX = MMin(MinX, SubFrustumLightSpace[i * 4 + j].x);
				MaxX = MMax(MaxX, SubFrustumLightSpace[i * 4 + j].x);
				MinY = MMin(MinY, SubFrustumLightSpace[i * 4 + j].y);
				MaxY = MMax(MaxY, SubFrustumLightSpace[i * 4 + j].y);
				MinZ = MMin(MinZ, SubFrustumLightSpace[i * 4 + j].z);
				MaxZ = MMax(MaxX, SubFrustumLightSpace[i * 4 + j].z);
			}
			ShadowInfo[i].ViewMatrix = getViewMatrix();
			float right = MaxX; float left = MinX;
			float top = MaxY; float bottom = MinY;
			float near = MinZ; float far = MaxZ;
			float A = 2.0 / (right - left);
			float B = 2.0 / (top - bottom);
			float C = -(right + left) / (right - left);
			float D = -(top + bottom) / (top - bottom);
			float q = -2 / (far - near);
			float qn = -(far + near) / (far - near);
			ShadowInfo[i].ProjectMatrix.SetZero();
			ShadowInfo[i].ProjectMatrix[0][0] = A;
			ShadowInfo[i].ProjectMatrix[0][3] = C;
			ShadowInfo[i].ProjectMatrix[1][1] = B;
			ShadowInfo[i].ProjectMatrix[1][3] = D;
			ShadowInfo[i].ProjectMatrix[2][2] = q;
			ShadowInfo[i].ProjectMatrix[2][3] = qn;
			ShadowInfo[i].ProjectMatrix[3][3] = 1;
		}
	}
}