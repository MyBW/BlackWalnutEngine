#include"BWPointLight.h"
#include"BWSceneManager.h"
BWPointLight::BWPointLight(const std::string &name, BWSceneManager* SceneManager) : BWLight(name, SceneManager , LT_POINT)
{
	mSceneManager->AddPointLight(this);
}

const BWVector3D& BWPointLight::GetWorldPosition() 
{
	BWMatrix4 Matrix;
	if (mParent)
	{
		getParentSceneNode()->getWorldTransforms(&Matrix);
		WorldPostion = Matrix.getTrans();
	}
	return WorldPostion;
}