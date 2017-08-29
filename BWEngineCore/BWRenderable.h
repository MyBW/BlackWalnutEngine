#ifndef RENDERABLE_H_
#define RENDERABLE_H_
#include "BWRenderQueue.h"
#include "BWTechnique.h"
#include "BWRenderOperation.h"
#include "BWPrimitive.h"
class BWRenderSystem;
class BWSceneManager;
class BWRenderable
{
public:
	BWRenderable();	
	~BWRenderable();
	RenderQueueGroup getRenderQueueDes() const;
	virtual BWMaterialPtr getMaterial() const = 0;
	virtual void getRenderOperation(BWRenderOperation &op) const  = 0;
	virtual BWTechnique* getTechnology() const;

	bool getUseIdentityView() const ;
	bool getUseIdentityProjection() const;

	virtual unsigned short getNumWorldTransforms() const ;
	virtual void getWorldTransforms(BWMatrix4* worldmatrix) const  = 0;

	virtual bool getPolygonModeOverrideable() const;
	virtual bool preRender(BWSceneManager* sceneManager, BWRenderSystem* renderSystem){ return true; }
	virtual void postRender(BWSceneManager *sceneManger, BWRenderSystem *renderSystem){ }
private:
	RenderQueueGroup renderDes;
	bool mPolygonModeOverrideable;
};

#endif // !RENDERABLE_H_
