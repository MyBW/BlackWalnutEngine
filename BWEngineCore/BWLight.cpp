#include "BWLight.h"
#include"BWSceneManager.h"
#include"BWTextureManager.h"
#include"BWCamera.h"
float Min(float a, float b) { return a > b ? b : a; }
float Max(float a, float b) { return a > b ? a : b; }


BWLight::BWLight(const std::string &name, BWSceneManager* scnenManager , LightType lightType) :BWFrustum(name, scnenManager)
,IsCastShadow(false),IsActive(true),mLightType(lightType)
{
}
BWLight::~BWLight()
{

}

bool BWLight::getCastShadows() const
{
	return false;
}

void BWLight::SetIsCastShadow(bool IsCast)
{
	IsCastShadow = IsCast;
}

const ColourValue& BWLight::getSpecularColour() const
{
	return ColourValue::Black;
}

const ColourValue& BWLight::getDiffuseColour() const
{
	return ColourValue::Black;
}

float BWLight::getAttenuationConstant() const
{
	return 0;
}

float BWLight::getAttenuationLinear() const
{
	return 0;
}
float BWLight::getAttenuationQuadric() const
{
	return 0;
}
float BWLight::getAttenuationRange() const
{
	return 0;
}

void BWLight::_updateCustomGpuParameter(unsigned short index, const BWGpuProgramParameters::AutoConstantEntry& constEntry, BWGpuProgramParameters *params) const
{

}

void BWLight::setAttenuation(float rang, float constant, float linear, float quadratic)
{

}

void BWLight::setDiffuseColour(const ColourValue& color)
{

}
void BWLight::setSpecularColour(const ColourValue& color)
{

}

BWLight::LightType BWLight::getType() const
{
	return mLightType;
}
float BWLight::getPowerScale() const
{
	return 0;
}
const BWVector3D& BWLight::getDerivedDirection() const
{
	return BWVector3D::UNIT_SCALE;
}
const BWVector3D& BWLight::getDerivedPosition(bool cameraRelative /* = false */) const
{
	return BWVector3D::UNIT_SCALE;
}
const BWEle4DD& BWLight::getAs4DVector(bool cameraRelative /* = false */) const
{
	BWEle4DD tmp;
	return tmp;
}

void BWLight::updateRenderQueue(BWRenderQueue *renderQueue)
{
	//mSceneManager->addActiveLight(this);
}

