#pragma once
#include"BWLight.h"

class BWPointLight : public BWLight
{
public:
	float getSpherePointRadius() { return RadiusScale; }
	void SetSpherePointRadius(float Radius) { RadiusScale = Radius; }
	const BWVector3& GetWorldPosition();

	BWPointLight(const std::string &name, BWSceneManager* SceneManager);
private:
	float RadiusScale;
	BWVector3 WorldPostion;
};