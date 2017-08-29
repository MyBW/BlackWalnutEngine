#include "BWRenderable.h"

BWRenderable::BWRenderable() :renderDes(MAIN_RENDERQUEUEGROUP),mPolygonModeOverrideable(false)
{

}
BWRenderable::~BWRenderable()
{

}
RenderQueueGroup BWRenderable::getRenderQueueDes() const 
{
	assert(0);
	return MAIN_RENDERQUEUEGROUP;
}

bool BWRenderable::getUseIdentityProjection() const
{
	return false;
}
unsigned short BWRenderable::getNumWorldTransforms() const
{
	return 1;
}
bool BWRenderable::getUseIdentityView() const
{
	return false;
}
BWMaterialPtr BWRenderable::getMaterial() const
{
	return NULL;
}
bool BWRenderable::getPolygonModeOverrideable() const
{
	return mPolygonModeOverrideable;
}
BWTechnique* BWRenderable::getTechnology() const
{
	return getMaterial()->getTechnique(0);
}
void BWRenderable::getWorldTransforms(BWMatrix4* worldmatrix) const
{
	*worldmatrix = BWMatrix4::IDENTITY;
}