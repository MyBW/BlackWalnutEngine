#pragma once
#include "BWLight.h"



class BWDirectionalLight : public BWLight
{
public:
	void SetDirection(const BWVector3D& Direction);
	const BWVector3D& GetDirection() const { return LightDirection; }
    BWDirectionalLight(const std::string &name, BWSceneManager* SceneManager);


	bool isViewMatrixOutOfDate() const override;
	const BWQuaternion& getOrientationForViewUpdate() const;
	const BWVector3D& getPositionForViewUpdate() const;
	void SetPosition(const BWVector3D& Position);


	void InitProjectedShadowMap(int MapIndex, BWHardwareDepthBufferPtr NewShadowMap, bool IsWithStencil = false);
	void InitProjectedShadowMap();
	void UpdateProjectedShadowMap(const BWCamera *Camera);
    FORCEINLINE bool SetCastShadowMapNum(int CastShadowMapNum);
	ShadowMapProjectInfo& GetShadowMapProjectedInfor(int ShadowMapIndex);
	FORCEINLINE int BWDirectionalLight::GetCastShadowMapNum() const { return  CastShadowMapNum; }
	BWVector4 FrustumEndWorldSpace[4];
	ShadowMapProjectInfo ShadowInfo[3];
	int ShadowMapSizeW;
	int ShadowMapSizeH;
protected:
	int CastShadowMapNum;
private:
	BWVector3D LightDirection;
	//相对于世界的方位和位置
	mutable BWQuaternion mRealOrientation;
	mutable BWVector3D mRealPosition;
	mutable bool mRecalcWindow;

	BWQuaternion mOrientation;
	BWVector3D mPosition;

	
};