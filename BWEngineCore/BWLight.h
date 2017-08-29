#ifndef LIGHT_H_
#define LIGHT_H_
#include"BWTexture.h"
#include"BWFrustum.h"
#include "BWGpuProgramParams.h"
#include"BWHardwareDepthBuffer.h"
#include"BWMovableObject.h"
struct ShadowMapProjectInfo
{
	BWMatrix4 ViewMatrix;
	BWMatrix4 ProjectMatrix;
	BWHardwareDepthBufferPtr ShadowTexture;
};
class BWLight : public BWFrustum
{
public:
	enum LightType
	{
		LT_POINT ,
		LT_DIRECTIONAL,
		LT_SPOTLIGHT
	};
public:
	void setDiffuseColour(const ColourValue& color);
	void setSpecularColour(const ColourValue& color);
	void setAttenuation(float rang, float constant, float linear, float quadratic);
	const ColourValue &getDiffuseColour() const ;
	const ColourValue &getSpecularColour() const ;
	float getPowerScale() const;
	const BWVector3 & getDerivedPosition(bool cameraRelative = false) const;
	const BWVector4 & getAs4DVector(bool cameraRelative = false) const ;
	const BWVector3 & getDerivedDirection() const ;
	float getAttenuationRange() const ;
	float getAttenuationConstant() const;
	float getAttenuationLinear() const;
	float getAttenuationQuadric() const;
	
	bool getCastShadows() const;
	void SetIsCastShadow(bool IsCast);
	size_t _getIndexInFrame() const { return mIndexInFrame; }
	void _updateCustomGpuParameter(unsigned short index, const BWGpuProgramParameters::AutoConstantEntry& constEntry, BWGpuProgramParameters *params) const ;

	//只是利用这个函数来把需要渲染的光源挑选出来
	virtual void updateRenderQueue(BWRenderQueue *renderQueue) override;

	LightType getType() const;
	void SetLightType(LightType LightType) { mLightType = LightType; }
	void SetLightColor(const ColourValue& Color) { LightColor = Color; }
	const ColourValue& GetLightColor() const { return LightColor; }
	
	BWLight(const std::string &name, BWSceneManager* scnenManager , LightType lightType);
	~BWLight();

	BWVector3D CameraPosition;
	BWCamera * tmp;
	float Roghness;
	float NormalReflectance;


	
protected:

	bool IsCastShadow;
private:
	size_t mIndexInFrame;
	std::string mName;
	LightType mLightType;
	ColourValue LightColor;
	bool IsActive;

	
};



#endif