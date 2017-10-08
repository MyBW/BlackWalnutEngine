#ifndef AUTOPARAMDATASOURCE_H_
#define AUTOPARAMDATASOURCE_H_
#include "BWLight.h"
#include "BWPrimitive.h"
#include "BWCommon.h"
#include "BWGpuProgramParams.h"
#include "BWRenderTarget.h"
#include "BWSceneManager.h"
#include "BWPass.h"
class BWFrustum;
class BWCamera;
class BWRenderable;


class BWAutoParamDataSource
{
protected:
	const BWLight& getLight(size_t index) const;
	mutable BWMatrix4 mWorldMatrix[256];
	mutable size_t mWorldMatrixCount;
	mutable const BWMatrix4* mWorldMatrixArray;
	mutable BWMatrix4 mWorldViewMatrix;
	mutable BWMatrix4 mViewProjMatrix;
	mutable BWMatrix4 mWorldViewProjMatrix;
	mutable BWMatrix4 mInverseWorldMatrix;
	mutable BWMatrix4 mInverseWorldViewMatrix;
	mutable BWMatrix4 mInverseViewMatrix;
	mutable BWMatrix4 mInverseTransposeWorldMatrix;
	mutable BWMatrix4 mInverseTransposeWorldViewMatrix;
	mutable BWVector4 mCameraPosition;
	mutable BWVector4 mCameraPositionObjectSpace;
	mutable BWMatrix4 mTextureViewProjMatrix[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable BWMatrix4 mTextureWorldViewProjMatrix[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable BWMatrix4 mSpotlightViewProjMatrix[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable BWMatrix4 mSpotlightWorldViewProjMatrix[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable BWVector4 mShadowCamDepthRanges[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable BWMatrix4 mViewMatrix;
	mutable BWMatrix4 mProjectionMatrix;
	mutable float mDirLightExtrusionDistance;
	mutable BWVector4 mLodCameraPosition;
	mutable BWVector4 mLodCameraPositionObjectSpace;

	mutable bool mWorldMatrixDirty;
	mutable bool mViewMatrixDirty;
	mutable bool mProjMatrixDirty;
	mutable bool mWorldViewMatrixDirty;
	mutable bool mViewProjMatrixDirty;
	mutable bool mWorldViewProjMatrixDirty;
	mutable bool mInverseWorldMatrixDirty;
	mutable bool mInverseWorldViewMatrixDirty;
	mutable bool mInverseViewMatrixDirty;
	mutable bool mInverseTransposeWorldMatrixDirty;
	mutable bool mInverseTransposeWorldViewMatrixDirty;
	mutable bool mCameraPositionDirty;
	mutable bool mCameraPositionObjectSpaceDirty;
	mutable bool mTextureViewProjMatrixDirty[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable bool mTextureWorldViewProjMatrixDirty[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable bool mSpotlightViewProjMatrixDirty[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable bool mSpotlightWorldViewProjMatrixDirty[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable bool mShadowCamDepthRangesDirty[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	mutable ColourValue mAmbientLight;
	mutable ColourValue mFogColour;
	mutable BWVector4 mFogParams;
	mutable int mPassNumber;
	mutable BWVector4 mSceneDepthRange;
	mutable bool mSceneDepthRangeDirty;
	mutable bool mLodCameraPositionDirty;
	mutable bool mLodCameraPositionObjectSpaceDirty;

	const BWRenderable* mCurrentRenderable;
    std::vector<const BWRenderable*> AllRenderableInThisFrame;
	std::map<const BWRenderable*, BWMatrix4> RenderablePreWolrdMatrix;
	int AllFrameNum{ 0 };
	float TemporalJitterPixelsX{ 0 };
	float TemporalJitterPixelsY{ 0 };
	const BWCamera* mCurrentCamera;
	bool mCameraRelativeRendering;
	BWVector3 mCameraRelativePosition;
	const LightList* mCurrentLightList;
	const BWFrustum* mCurrentTextureProjector[OGRE_MAX_SIMULTANEOUS_LIGHTS];
	const BWRenderTarget* mCurrentRenderTarget;
	const BWViewport* mCurrentViewport;
	const BWSceneManager* mCurrentSceneManager;
	const VisibleObjectsBoundsInfo* mMainCamBoundsInfo;
	const BWPass* mCurrentPass;

	template<typename T>
	struct TActiveLightMap
	{
	};
	template<>
	struct TActiveLightMap<DirectionalLightMap>
	{
		static DirectionalLightMap* ActiveLightMap;
	};
	template<>
	struct TActiveLightMap<LightMap>
	{
		static LightMap* ActiveLightMap;
	};
	template<>
	struct TActiveLightMap<PointLightMap>
	{
		static PointLightMap* ActiveLightMap;
	};

	ObjectsQueneWithInLightMap *ObjectsQuenesWithInLight;
public:
	template<typename T>
	FORCEINLINE auto GetActiveLightMap()->decltype(TActiveLightMap<T>::ActiveLightMap)
	{
		return TActiveLightMap<T>::ActiveLightMap ;
	}
	template<typename T>
	FORCEINLINE void SetActiveLightMap(T& LightsMap)
	{
		TActiveLightMap<T>::ActiveLightMap = &LightsMap;
	}
	FORCEINLINE void SetObejctesQueueInLight(ObjectsQueneWithInLightMap& ObjectsQuenesWithInLight) { this->ObjectsQuenesWithInLight = &ObjectsQuenesWithInLight; }

	FORCEINLINE ObjectsQueneWithInLightMap* GetObjectesQueueInLight() { return ObjectsQuenesWithInLight; }

	BWAutoParamDataSource();
	virtual ~BWAutoParamDataSource();
	virtual void EndFrame();
	/** Updates the current renderable */
	virtual void setCurrentRenderable(const BWRenderable* rend);
	/** Sets the world matrices, avoid query from renderable again */
	virtual void setWorldMatrices(const BWMatrix4* m, size_t count);
	/** Updates the current camera */
	virtual void setCurrentCamera(const BWCamera* cam, bool useCameraRelative);
	/** Sets the light list that should be used, and it's base index from the global list */
	virtual void setCurrentLightList(const LightList* ll);
	/** Sets the current texture projector for a index */
	virtual void setTextureProjector(const BWFrustum* frust, size_t index);
	/** Sets the current render target */
	virtual void setCurrentRenderTarget(const BWRenderTarget* target);
	/** Sets the current viewport */
	virtual void setCurrentViewport(const BWViewport* viewport);
	/** Sets the shadow extrusion distance to be used for point lights. */
	virtual void setShadowDirLightExtrusionDistance(float dist);
	/** Sets the main camera's scene bounding information */
	virtual void setMainCamBoundsInfo(VisibleObjectsBoundsInfo* info);
	/** Set the current scene manager for enquiring on demand */
	virtual void setCurrentSceneManager(const BWSceneManager* sm);
	/** Sets the current pass */
	virtual void setCurrentPass(const BWPass* pass);

	virtual const BWCamera* GetActiveCamera() { return mCurrentCamera; }

	virtual const BWMatrix4& getWorldMatrix(void) const;
	virtual const BWMatrix4& getPreWorldMatrix(void)const;
	virtual const BWMatrix4* getWorldMatrixArray(void) const;
	virtual size_t getWorldMatrixCount(void) const;
	virtual const BWMatrix4& getViewMatrix(void) const;
	virtual const BWMatrix4& getPreViewMatrix(void) const;
	virtual const BWMatrix4& getViewProjectionMatrix(void) const;
	virtual const BWMatrix4& getPreProjectionMatrix(void) const;
	virtual const BWMatrix4& getProjectionMatrix(void) const;
	virtual const BWMatrix4& getWorldViewProjMatrix(void) const;
	virtual const BWMatrix4& getWorldViewMatrix(void) const;
	virtual const BWMatrix4& getInverseWorldMatrix(void) const;
	virtual const BWMatrix4& getInverseWorldViewMatrix(void) const;
	virtual const BWMatrix4& getInverseViewMatrix(void) const;
	virtual const BWMatrix4& getInverseTransposeWorldMatrix(void) const;
	virtual const BWMatrix4& getInverseTransposeWorldViewMatrix(void) const;
	virtual const BWVector4& getCameraPosition(void) const;
	virtual const BWVector4& getCameraPositionObjectSpace(void) const;
	virtual const BWVector4& getLodCameraPosition(void) const;
	virtual const BWVector4& getLodCameraPositionObjectSpace(void) const;
	virtual bool hasLightList() const { return mCurrentLightList != 0; }
	/** Get the light which is 'index'th closest to the current object */
	virtual float getLightNumber(size_t index) const;
	virtual float getLightCount() const;
	virtual float getLightCastsShadows(size_t index) const;
	virtual const ColourValue& getLightDiffuseColour(size_t index) const;
	virtual const ColourValue& getLightSpecularColour(size_t index) const;
	virtual const ColourValue getLightDiffuseColourWithPower(size_t index) const;
	virtual const ColourValue getLightSpecularColourWithPower(size_t index) const;
	virtual const BWVector3& getLightPosition(size_t index) const;
	virtual BWVector4 getLightAs4DVector(size_t index) const;
	virtual const BWVector3& getLightDirection(size_t index) const;
	virtual float getLightPowerScale(size_t index) const;
	virtual BWVector4 getLightAttenuation(size_t index) const;
	virtual BWVector4 getSpotlightParams(size_t index) const;
	virtual void setAmbientLightColour(const ColourValue& ambient);
	virtual const ColourValue& getAmbientLightColour(void) const;
	virtual const ColourValue& getSurfaceAmbientColour(void) const;
	virtual const ColourValue& getSurfaceDiffuseColour(void) const;
	virtual const ColourValue& getSurfaceSpecularColour(void) const;
	virtual const ColourValue& getSurfaceEmissiveColour(void) const;
	virtual float getSurfaceShininess(void) const;
	virtual ColourValue getDerivedAmbientLightColour(void) const;
	virtual ColourValue getDerivedSceneColour(void) const;
	virtual void setFog(FogMode mode, const ColourValue& colour, float expDensity, float linearStart, float linearEnd);
	virtual const ColourValue& getFogColour(void) const;
	virtual const BWVector4& getFogParams(void) const;
	virtual const BWMatrix4& getTextureViewProjMatrix(size_t index) const;
	virtual const BWMatrix4& getTextureWorldViewProjMatrix(size_t index) const;
	virtual const BWMatrix4& getSpotlightViewProjMatrix(size_t index) const;
	virtual const BWMatrix4& getSpotlightWorldViewProjMatrix(size_t index) const;
	virtual const BWMatrix4& getTextureTransformMatrix(size_t index) const;
	virtual const BWRenderTarget* getCurrentRenderTarget(void) const;
	virtual const BWRenderable* getCurrentRenderable(void) const;
	virtual const BWPass* getCurrentPass(void) const;
	virtual BWVector4 getTextureSize(size_t index) const;
	virtual BWVector4 getInverseTextureSize(size_t index) const;
	virtual BWVector4 getPackedTextureSize(size_t index) const;
	virtual float getShadowExtrusionDistance(void) const;
	virtual const BWVector4& getSceneDepthRange() const;
	virtual const BWVector4& getShadowSceneDepthRange(size_t index) const;
	virtual const ColourValue& getShadowColour() const;
	virtual BWMatrix4 getInverseViewProjMatrix(void) const;
	virtual BWMatrix4 getInverseTransposeViewProjMatrix() const;
	virtual BWMatrix4 getTransposeViewProjMatrix() const;
	virtual BWMatrix4 getTransposeViewMatrix() const;
	virtual BWMatrix4 getInverseTransposeViewMatrix() const;
	virtual BWMatrix4 getTransposeProjectionMatrix() const;
	virtual BWMatrix4 getInverseProjectionMatrix() const;
	virtual BWMatrix4 getInverseTransposeProjectionMatrix() const;
	virtual BWMatrix4 getTransposeWorldViewProjMatrix() const;
	virtual BWMatrix4 getInverseWorldViewProjMatrix() const;
	virtual BWMatrix4 getInverseTransposeWorldViewProjMatrix() const;
	virtual BWMatrix4 getTransposeWorldViewMatrix() const;
	virtual BWMatrix4 getTransposeWorldMatrix() const;
	virtual float getTime(void) const;
	virtual float getTime_0_X(float x) const;
	virtual float getCosTime_0_X(float x) const;
	virtual float getSinTime_0_X(float x) const;
	virtual float getTanTime_0_X(float x) const;
	virtual BWVector4 getTime_0_X_packed(float x) const;
	virtual float getTime_0_1(float x) const;
	virtual float getCosTime_0_1(float x) const;
	virtual float getSinTime_0_1(float x) const;
	virtual float getTanTime_0_1(float x) const;
	virtual BWVector4 getTime_0_1_packed(float x) const;
	virtual float getTime_0_2Pi(float x) const;
	virtual float getCosTime_0_2Pi(float x) const;
	virtual float getSinTime_0_2Pi(float x) const;
	virtual float getTanTime_0_2Pi(float x) const;
	virtual BWVector4 getTime_0_2Pi_packed(float x) const;
	virtual float getFrameTime(void) const;
	virtual float getFPS() const;
	virtual float getViewportWidth() const;
	virtual float getViewportHeight() const;
	virtual float getInverseViewportWidth() const;
	virtual float getInverseViewportHeight() const;
	virtual BWVector3 getViewDirection() const;
	virtual BWVector3 getViewSideVector() const;
	virtual BWVector3 getViewUpVector() const;
	virtual float getFOV() const;
	virtual float getNearClipDistance() const;
	virtual float getFarClipDistance() const;
	FORCEINLINE float GetTemporalAAJitterX() const { return TemporalJitterPixelsX; }
	FORCEINLINE float GetTemporalAAJitterY() const { return TemporalJitterPixelsY; }
	virtual int getPassNumber(void) const;
	virtual void setPassNumber(const int passNumber);
	virtual void incPassNumber(void);
	virtual void updateLightCustomGpuParameter(const BWGpuProgramParameters::AutoConstantEntry& constantEntry, BWGpuProgramParameters *params) const;
	virtual void updateActivePass();
	void SetGPUAutoParameter(BWGpuProgramParametersPtr GPUProgramParameter);

};

#endif