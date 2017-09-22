#include "BWAutoParamDataSource.h"
#include "BWColorValue.h"
#include "BWPass.h"
#include "BWRenderSystem.h"
#include "BWRoot.h"
#include "Math.h"
#include "BWCamera.h"
#include <utility>



DirectionalLightMap* BWAutoParamDataSource::TActiveLightMap<DirectionalLightMap>::ActiveLightMap = nullptr;
LightMap* BWAutoParamDataSource::TActiveLightMap<LightMap>::ActiveLightMap = nullptr;
PointLightMap* BWAutoParamDataSource::TActiveLightMap<PointLightMap>::ActiveLightMap = nullptr;

	const BWMatrix4 PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE(
		0.5, 0, 0, 0.5,
		0, -0.5, 0, 0.5,
		0, 0, 1, 0,
		0, 0, 0, 1);

	//-----------------------------------------------------------------------------
	BWAutoParamDataSource::BWAutoParamDataSource()
		: mWorldMatrixDirty(true),
		mViewMatrixDirty(true),
		mProjMatrixDirty(true),
		mWorldViewMatrixDirty(true),
		mViewProjMatrixDirty(true),
		mWorldViewProjMatrixDirty(true),
		mInverseWorldMatrixDirty(true),
		mInverseWorldViewMatrixDirty(true),
		mInverseViewMatrixDirty(true),
		mInverseTransposeWorldMatrixDirty(true),
		mInverseTransposeWorldViewMatrixDirty(true),
		mCameraPositionDirty(true),
		mCameraPositionObjectSpaceDirty(true),
		mSceneDepthRangeDirty(true),
		mLodCameraPositionDirty(true),
		mLodCameraPositionObjectSpaceDirty(true),
		mCurrentRenderable(0),
		mCurrentCamera(0),
		mCameraRelativeRendering(false),
		mCurrentLightList(0),
		mCurrentRenderTarget(0),
		mCurrentViewport(0),
		mCurrentSceneManager(0),
		mMainCamBoundsInfo(0),
		mCurrentPass(0)
	{
		/*mBlankLight.setDiffuseColour(ColourValue::Black);
		mBlankLight.setSpecularColour(ColourValue::Black);
		mBlankLight.setAttenuation(0, 1, 0, 0);
		*/
		for (size_t i = 0; i < OGRE_MAX_SIMULTANEOUS_LIGHTS; ++i)
		{
			mTextureViewProjMatrixDirty[i] = true;
			mTextureWorldViewProjMatrixDirty[i] = true;
			mSpotlightViewProjMatrixDirty[i] = true;
			mSpotlightWorldViewProjMatrixDirty[i] = true;
			mCurrentTextureProjector[i] = 0;
			mShadowCamDepthRangesDirty[i] = false;
		}

	}
	//-----------------------------------------------------------------------------
	BWAutoParamDataSource::~BWAutoParamDataSource()
	{
	}

	void BWAutoParamDataSource::EndFrame()
	{
		mCurrentCamera->EndFrame();
		RenderablePreWolrdMatrix.clear();
		for (auto Renderable : AllRenderableInThisFrame)
		{
			BWMatrix4 WorldTrans;
			Renderable->getWorldTransforms(&WorldTrans);
			RenderablePreWolrdMatrix.insert(std::pair<BWRenderable*, BWMatrix4>(Renderable, WorldTrans));
		}
	}

	//-----------------------------------------------------------------------------
	const BWLight& BWAutoParamDataSource::getLight(size_t index) const
	{
		// If outside light range, return a blank light to ensure zeroised for program
		/*if (mCurrentLightList && index < mCurrentLightList->size())
		{
			return *((*mCurrentLightList)[index]);
		}
		else
		{
			return mBlankLight;
		}*/
		BWLight tmpLgiht("test", NULL , BWLight::LightType::LT_DIRECTIONAL);
		return tmpLgiht;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentRenderable(const BWRenderable* rend)
	{
		mCurrentRenderable = nullptr;
		for (auto Ele : AllRenderableInThisFrame)
		{
			if (Ele == rend) mCurrentRenderable = Ele;
		}
		if (!mCurrentRenderable)
		{
			mCurrentRenderable = rend;
			AllRenderableInThisFrame.push_back(rend);
		}
		mWorldMatrixDirty = true;
		mViewMatrixDirty = true;
		mProjMatrixDirty = true;
		mWorldViewMatrixDirty = true;
		mViewProjMatrixDirty = true;
		mWorldViewProjMatrixDirty = true;
		mInverseWorldMatrixDirty = true;
		mInverseViewMatrixDirty = true;
		mInverseWorldViewMatrixDirty = true;
		mInverseTransposeWorldMatrixDirty = true;
		mInverseTransposeWorldViewMatrixDirty = true;
		mCameraPositionObjectSpaceDirty = true;
		mLodCameraPositionObjectSpaceDirty = true;
		for (size_t i = 0; i < OGRE_MAX_SIMULTANEOUS_LIGHTS; ++i)
		{
			mTextureWorldViewProjMatrixDirty[i] = true;
			mSpotlightWorldViewProjMatrixDirty[i] = true;
		}

		
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentCamera(const BWCamera* cam, bool useCameraRelative)
	{
		mCurrentCamera = cam;
		mCameraRelativeRendering = useCameraRelative;
		mCameraRelativePosition = cam->getDerivedPosition();
		mViewMatrixDirty = true;
		mProjMatrixDirty = true;
		mWorldViewMatrixDirty = true;
		mViewProjMatrixDirty = true;
		mWorldViewProjMatrixDirty = true;
		mInverseViewMatrixDirty = true;
		mInverseWorldViewMatrixDirty = true;
		mInverseTransposeWorldViewMatrixDirty = true;
		mCameraPositionObjectSpaceDirty = true;
		mCameraPositionDirty = true;
		mLodCameraPositionObjectSpaceDirty = true;
		mLodCameraPositionDirty = true;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentLightList(const LightList* ll)
	{
		mCurrentLightList = ll;
		for (size_t i = 0; i < ll->size() && i < OGRE_MAX_SIMULTANEOUS_LIGHTS; ++i)
		{
			mSpotlightViewProjMatrixDirty[i] = true;
			mSpotlightWorldViewProjMatrixDirty[i] = true;
		}

	}
	//---------------------------------------------------------------------
	float BWAutoParamDataSource::getLightNumber(size_t index) const
	{
		return static_cast<float>(getLight(index)._getIndexInFrame());
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getLightDiffuseColour(size_t index) const
	{
		return getLight(index).getDiffuseColour();
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getLightSpecularColour(size_t index) const
	{
		return getLight(index).getSpecularColour();
	}
	//-----------------------------------------------------------------------------
	const ColourValue BWAutoParamDataSource::getLightDiffuseColourWithPower(size_t index) const
	{
		const BWLight& l = getLight(index);
		ColourValue scaled(l.getDiffuseColour());
		float power = l.getPowerScale();
		// scale, but not alpha
		scaled.r *= power;
		scaled.g *= power;
		scaled.b *= power;
		return scaled;
	}
	//-----------------------------------------------------------------------------
	const ColourValue BWAutoParamDataSource::getLightSpecularColourWithPower(size_t index) const
	{
		const BWLight& l = getLight(index);
		ColourValue scaled(l.getSpecularColour());
		float power = l.getPowerScale();
		// scale, but not alpha
		scaled.r *= power;
		scaled.g *= power;
		scaled.b *= power;
		return scaled;
	}
	//-----------------------------------------------------------------------------
	const BWVector3& BWAutoParamDataSource::getLightPosition(size_t index) const
	{
		return getLight(index).getDerivedPosition(true);
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getLightAs4DVector(size_t index) const
	{
		return getLight(index).getAs4DVector(true);
	}
	//-----------------------------------------------------------------------------
	const BWVector3& BWAutoParamDataSource::getLightDirection(size_t index) const
	{
		return getLight(index).getDerivedDirection();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getLightPowerScale(size_t index) const
	{
		return getLight(index).getPowerScale();
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getLightAttenuation(size_t index) const
	{
		// range, const, linear, quad
		const BWLight& l = getLight(index);
		return BWVector4(l.getAttenuationRange(),
			l.getAttenuationConstant(),
			l.getAttenuationLinear(),
			l.getAttenuationQuadric());
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getSpotlightParams(size_t index) const
	{
		// inner, outer, fallof, isSpot
		const BWLight& l = getLight(index);
		if (l.getType() == BWLight::LT_SPOTLIGHT)
		{/*
			return BWVector4(Math::Cos(l.getSpotlightInnerAngle().valueRadians() * 0.5f),
				Math::Cos(l.getSpotlightOuterAngle().valueRadians() * 0.5f),
				l.getSpotlightFalloff(),
				1.0);*/
			assert(0);
		}
		else
		{
			// Use safe values which result in no change to point & dir light calcs
			// The spot factor applied to the usual lighting calc is 
			// pow((dot(spotDir, lightDir) - y) / (x - y), z)
			// Therefore if we set z to 0.0f then the factor will always be 1
			// since pow(anything, 0) == 1
			// However we also need to ensure we don't overflow because of the division
			// therefore set x = 1 and y = 0 so divisor doesn't change scale
			return BWVector4(1.0, 0.0, 0.0, 1.0); // since the main op is pow(.., vec4.z), this will result in 1.0
		}
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setMainCamBoundsInfo(VisibleObjectsBoundsInfo* info)
	{
		mMainCamBoundsInfo = info;
		mSceneDepthRangeDirty = true;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentSceneManager(const BWSceneManager* sm)
	{
		mCurrentSceneManager = sm;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setWorldMatrices(const BWMatrix4* m, size_t count)
	{
		mWorldMatrixArray = m;
		mWorldMatrixCount = count;
		mWorldMatrixDirty = false;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getWorldMatrix(void) const
	{
		if (mWorldMatrixDirty)
		{
			mWorldMatrixArray = mWorldMatrix;
			mCurrentRenderable->getWorldTransforms(mWorldMatrix);
			mWorldMatrixCount = mCurrentRenderable->getNumWorldTransforms();
			if (mCameraRelativeRendering)
			{
				for (size_t i = 0; i < mWorldMatrixCount; ++i)
				{
					mWorldMatrix[i].setTrans(mWorldMatrix[i].getTrans() - mCameraRelativePosition);
				}
			}
			mWorldMatrixDirty = false;
		}
		return mWorldMatrixArray[0];
	}

	const BWMatrix4& BWAutoParamDataSource::getPreWorldMatrix(void) const
	{
		std::map<const BWRenderable*, BWMatrix4>::const_iterator Itor = RenderablePreWolrdMatrix.find(mCurrentRenderable);
		return Itor->second;
	}

	//-----------------------------------------------------------------------------
	size_t BWAutoParamDataSource::getWorldMatrixCount(void) const
	{
		// trigger derivation
		getWorldMatrix();
		return mWorldMatrixCount;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4* BWAutoParamDataSource::getWorldMatrixArray(void) const
	{
		// trigger derivation
		getWorldMatrix();
		return mWorldMatrixArray;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getViewMatrix(void) const
	{
		if (mViewMatrixDirty)
		{
			if (mCurrentRenderable && mCurrentRenderable->getUseIdentityView())
				mViewMatrix = BWMatrix4::IDENTITY;
			else
			{
				mViewMatrix = mCurrentCamera->getViewMatrix(true);
				if (mCameraRelativeRendering)
				{
					mViewMatrix.setTrans(BWVector3::ZERO);
				}

			}
			mViewMatrixDirty = false;
		}
		return mViewMatrix;
	}

	const BWMatrix4& BWAutoParamDataSource::getPreViewMatrix(void) const
	{
		return  mCurrentCamera->getPreViewMatrix(true);
	}

	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getViewProjectionMatrix(void) const
	{
		if (mViewProjMatrixDirty)
		{
			mViewProjMatrix = getProjectionMatrix() * getViewMatrix();
			mViewProjMatrixDirty = false;
		}
		return mViewProjMatrix;
	}

	const BWMatrix4& BWAutoParamDataSource::getPreProjectionMatrix(void) const
	{
		return mCurrentCamera->getPreProjectionMatrixRS();
	}

	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getProjectionMatrix(void) const
	{
		if (mProjMatrixDirty)
		{
			// NB use API-independent projection matrix since GPU programs	
			// bypass the API-specific handedness and use right-handed coords
			if (mCurrentRenderable && mCurrentRenderable->getUseIdentityProjection())
			{
				// Use identity projection matrix, still need to take RS depth into account.
				BWRenderSystem* rs = BWRoot::GetInstance()->getRenderSystem();
				rs->_convertProjectionMatrix(BWMatrix4::IDENTITY, mProjectionMatrix, true);
			}
			else
			{
				//mProjectionMatrix = mCurrentCamera->getProjectionMatrixWithRSDepth();
				mProjectionMatrix = mCurrentCamera->getProjectionMatrixRS();
			}
			if (mCurrentRenderTarget && mCurrentRenderTarget->requiresTextureFlipping())
			{
				// Because we're not using setProjectionMatrix, this needs to be done here
				// Invert transformed y
				mProjectionMatrix[1][0] = -mProjectionMatrix[1][0];
				mProjectionMatrix[1][1] = -mProjectionMatrix[1][1];
				mProjectionMatrix[1][2] = -mProjectionMatrix[1][2];
				mProjectionMatrix[1][3] = -mProjectionMatrix[1][3];
			}
			mProjMatrixDirty = false;
		}
		return mProjectionMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getWorldViewMatrix(void) const
	{
		if (mWorldViewMatrixDirty)
		{
			mWorldViewMatrix = getViewMatrix().concatenateAffine(getWorldMatrix());
			mWorldViewMatrixDirty = false;
		}
		return mWorldViewMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getWorldViewProjMatrix(void) const
	{
		if (mWorldViewProjMatrixDirty)
		{
			mWorldViewProjMatrix =  getProjectionMatrix() * getWorldViewMatrix();
			mWorldViewProjMatrixDirty = false;
		}
		return mWorldViewProjMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getInverseWorldMatrix(void) const
	{
		if (mInverseWorldMatrixDirty)
		{
			mInverseWorldMatrix = getWorldMatrix().inverseAffine();
			mInverseWorldMatrixDirty = false;
		}
		return mInverseWorldMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getInverseWorldViewMatrix(void) const
	{
		if (mInverseWorldViewMatrixDirty)
		{
			mInverseWorldViewMatrix = getWorldViewMatrix().inverseAffine();
			mInverseWorldViewMatrixDirty = false;
		}
		return mInverseWorldViewMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getInverseViewMatrix(void) const
	{
		if (mInverseViewMatrixDirty)
		{
			mInverseViewMatrix = getViewMatrix().inverseAffine();
			mInverseViewMatrixDirty = false;
		}
		return mInverseViewMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getInverseTransposeWorldMatrix(void) const
	{
		if (mInverseTransposeWorldMatrixDirty)
		{
			mInverseTransposeWorldMatrix = getInverseWorldMatrix().transpose();
			mInverseTransposeWorldMatrixDirty = false;
		}
		return mInverseTransposeWorldMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getInverseTransposeWorldViewMatrix(void) const
	{
		if (mInverseTransposeWorldViewMatrixDirty)
		{
			mInverseTransposeWorldViewMatrix = getInverseWorldViewMatrix().transpose();
			mInverseTransposeWorldViewMatrixDirty = false;
		}
		return mInverseTransposeWorldViewMatrix;
	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getCameraPosition(void) const
	{
		if (mCameraPositionDirty)
		{
			BWVector3 vec3 = mCurrentCamera->getDerivedPosition();
			if (mCameraRelativeRendering)
			{
				vec3 = vec3 - mCameraRelativePosition;
			}
			mCameraPosition[0] = vec3[0];
			mCameraPosition[1] = vec3[1];
			mCameraPosition[2] = vec3[2];
			mCameraPosition[3] = 1.0;
			mCameraPositionDirty = false;
		}
		return mCameraPosition;
	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getCameraPositionObjectSpace(void) const
	{
		if (mCameraPositionObjectSpaceDirty)
		{
			if (mCameraRelativeRendering)
			{
				mCameraPositionObjectSpace =
					getInverseWorldMatrix().transformAffine(BWVector3::ZERO);
			}
			else
			{
				mCameraPositionObjectSpace =
					getInverseWorldMatrix().transformAffine(mCurrentCamera->getDerivedPosition());
			}
			mCameraPositionObjectSpaceDirty = false;
		}
		return mCameraPositionObjectSpace;
	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getLodCameraPosition(void) const
	{
		if (mLodCameraPositionDirty)
		{
			BWVector3 vec3 = mCurrentCamera->getLodCamera()->getDerivedPosition();
			if (mCameraRelativeRendering)
			{
				vec3 =vec3 - mCameraRelativePosition;
			}
			mLodCameraPosition[0] = vec3[0];
			mLodCameraPosition[1] = vec3[1];
			mLodCameraPosition[2] = vec3[2];
			mLodCameraPosition[3] = 1.0;
			mLodCameraPositionDirty = false;
		}
		return mLodCameraPosition;
	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getLodCameraPositionObjectSpace(void) const
	{
		if (mLodCameraPositionObjectSpaceDirty)
		{
			if (mCameraRelativeRendering)
			{
				mLodCameraPositionObjectSpace =
					getInverseWorldMatrix().transformAffine(mCurrentCamera->getLodCamera()->getDerivedPosition()
					- mCameraRelativePosition);
			}
			else
			{
				mLodCameraPositionObjectSpace =
					getInverseWorldMatrix().transformAffine(mCurrentCamera->getLodCamera()->getDerivedPosition());
			}
			mLodCameraPositionObjectSpaceDirty = false;
		}
		return mLodCameraPositionObjectSpace;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setAmbientLightColour(const ColourValue& ambient)
	{
		mAmbientLight = ambient;
	}
	//---------------------------------------------------------------------
	float BWAutoParamDataSource::getLightCount() const
	{
		return static_cast<float>(mCurrentLightList->size());
	}
	//---------------------------------------------------------------------
	float BWAutoParamDataSource::getLightCastsShadows(size_t index) const
	{
		return getLight(index).getCastShadows() ? 1.0f : 0.0f;
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getAmbientLightColour(void) const
	{
		return mAmbientLight;

	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentPass(const BWPass* pass)
	{
		mCurrentPass = pass;
	}
	//-----------------------------------------------------------------------------
	const BWPass* BWAutoParamDataSource::getCurrentPass(void) const
	{
		return mCurrentPass;
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getTextureSize(size_t index) const
	{
		BWVector4 size = BWVector4(1, 1, 1, 1);

		if (index < mCurrentPass->getNumTextureUnitStates())
		{
			const BWTexturePtr& tex = mCurrentPass->getTextureUnitState(
				static_cast<unsigned short>(index))->_getTexturePtr();
			if (!tex.IsNull())
			{
				size.x = static_cast<float>(tex->getWidth());
				size.y = static_cast<float>(tex->getHeight());
				size.z = static_cast<float>(tex->getDepth());
			}
		}

		return size;
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getInverseTextureSize(size_t index) const
	{
		BWVector4 size = getTextureSize(index);
		size.x = 1 / size.x;
		size.y = 1 / size.y;
		size.z = 1 / size.z;
		size.w = 1 / size.w;
		return size;
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getPackedTextureSize(size_t index) const
	{
		BWVector4 size = getTextureSize(index);
		return BWVector4(size.x, size.y, 1 / size.x, 1 / size.y);
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getSurfaceAmbientColour(void) const
	{
		return mCurrentPass->getAmbient();
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getSurfaceDiffuseColour(void) const
	{
		return mCurrentPass->getDiffuse();
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getSurfaceSpecularColour(void) const
	{
		return mCurrentPass->getSpecular();
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getSurfaceEmissiveColour(void) const
	{
		return mCurrentPass->getSelfIllumination();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getSurfaceShininess(void) const
	{
		return mCurrentPass->getShininess();
	}
	//-----------------------------------------------------------------------------
	ColourValue BWAutoParamDataSource::getDerivedAmbientLightColour(void) const
	{
		return getAmbientLightColour() * getSurfaceAmbientColour();
	}
	//-----------------------------------------------------------------------------
	ColourValue BWAutoParamDataSource::getDerivedSceneColour(void) const
	{
		ColourValue result = getDerivedAmbientLightColour() + getSurfaceEmissiveColour();
		result.a = getSurfaceDiffuseColour().a;
		return result;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setFog(FogMode mode, const ColourValue& colour,
		float expDensity, float linearStart, float linearEnd)
	{
		(void)mode; // ignored
		mFogColour = colour;
		mFogParams.x = expDensity;
		mFogParams.y = linearStart;
		mFogParams.z = linearEnd;
		mFogParams.w = linearEnd != linearStart ? 1 / (linearEnd - linearStart) : 0;
	}
	//-----------------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getFogColour(void) const
	{
		return mFogColour;
	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getFogParams(void) const
	{
		return mFogParams;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setTextureProjector(const BWFrustum* frust, size_t index = 0)
	{
		if (index < OGRE_MAX_SIMULTANEOUS_LIGHTS)
		{
			mCurrentTextureProjector[index] = frust;
			mTextureViewProjMatrixDirty[index] = true;
			mTextureWorldViewProjMatrixDirty[index] = true;
			mShadowCamDepthRangesDirty[index] = true;
		}

	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getTextureViewProjMatrix(size_t index) const
	{
		if (index < OGRE_MAX_SIMULTANEOUS_LIGHTS)
		{
			if (mTextureViewProjMatrixDirty[index] && mCurrentTextureProjector[index])
			{
				if (mCameraRelativeRendering)
				{
					// World positions are now going to be relative to the camera position
					// so we need to alter the projector view matrix to compensate
					BWMatrix4 viewMatrix;
					mCurrentTextureProjector[index]->calcViewMatrixRelative(
						mCurrentCamera->getDerivedPosition(), viewMatrix);
					mTextureViewProjMatrix[index] =
						PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE *
						mCurrentTextureProjector[index]->getProjectionMatrixWithRSDepth() *
						viewMatrix;
				}
				else
				{
					mTextureViewProjMatrix[index] =
						PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE *
						mCurrentTextureProjector[index]->getProjectionMatrixWithRSDepth() *
						mCurrentTextureProjector[index]->getViewMatrix();
				}
				mTextureViewProjMatrixDirty[index] = false;
			}
			return mTextureViewProjMatrix[index];
		}
		else
			return BWMatrix4::IDENTITY;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getTextureWorldViewProjMatrix(size_t index) const
	{
		if (index < OGRE_MAX_SIMULTANEOUS_LIGHTS)
		{
			if (mTextureWorldViewProjMatrixDirty[index] && mCurrentTextureProjector[index])
			{
				mTextureWorldViewProjMatrix[index] =
					getTextureViewProjMatrix(index)	* getWorldMatrix();
				mTextureWorldViewProjMatrixDirty[index] = false;
			}
			return mTextureWorldViewProjMatrix[index];
		}
		else
			return BWMatrix4::IDENTITY;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getSpotlightViewProjMatrix(size_t index) const
	{
		assert(0);
		//if (index < OGRE_MAX_SIMULTANEOUS_LIGHTS)
		//{
		//	const BWLight& l = getLight(index);

		//	if (&l != &mBlankLight &&
		//		l.getType() == BWLight::LT_SPOTLIGHT &&
		//		mSpotlightViewProjMatrixDirty[index])
		//	{
		//		BWFrustum frust;
		//		BWSceneNode dummyNode(0);
		//		dummyNode.attachObject(&frust);

		//		frust.setProjectionType(PT_PERSPECTIVE);
		//		frust.setFOVy(l.getSpotlightOuterAngle());
		//		frust.setAspectRatio(1.0f);
		//		// set near clip the same as main camera, since they are likely
		//		// to both reflect the nature of the scene
		//		frust.setNearClipDistance(mCurrentCamera->getNearClipDistance());
		//		// Calculate position, which same as spotlight position, in camera-relative coords if required
		//		dummyNode.setPosition(l.getDerivedPosition(true));
		//		// Calculate direction, which same as spotlight direction
		//		BWVector3 dir = -l.getDerivedDirection(); // backwards since point down -z
		//		dir.normalise();
		//		BWVector3 up = BWVector3::UNIT_Y;
		//		// Check it's not coincident with dir
		//		if (Math::Abs(up.dotProduct(dir)) >= 1.0f)
		//		{
		//			// Use camera up
		//			up = BWVector3::UNIT_Z;
		//		}
		//		// cross twice to rederive, only direction is unaltered
		//		BWVector3 left = dir.crossProduct(up);
		//		left.normalise();
		//		up = dir.crossProduct(left);
		//		up.normalise();
		//		// Derive quaternion from axes
		//		Quaternion q;
		//		q.FromAxes(left, up, dir);
		//		dummyNode.setOrientation(q);

		//		// The view matrix here already includes camera-relative changes if necessary
		//		// since they are built into the frustum position
		//		mSpotlightViewProjMatrix[index] =
		//			PROJECTIONCLIPSPACE2DTOIMAGESPACE_PERSPECTIVE *
		//			frust.getProjectionMatrixWithRSDepth() *
		//			frust.getViewMatrix();

		//		mSpotlightViewProjMatrixDirty[index] = false;
		//	}
		//	return mSpotlightViewProjMatrix[index];
		//}
		//else
		//	return BWMatrix4::IDENTITY;
		return BWMatrix4::IDENTITY;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getSpotlightWorldViewProjMatrix(size_t index) const
	{
		//if (index < OGRE_MAX_SIMULTANEOUS_LIGHTS)
		//{
		//	const BWLight& l = getLight(index);

		//	if (&l != &mBlankLight &&
		//		l.getType() == BWLight::LT_SPOTLIGHT &&
		//		mSpotlightWorldViewProjMatrixDirty[index])
		//	{
		//		mSpotlightWorldViewProjMatrix[index] =
		//			getSpotlightViewProjMatrix(index) * getWorldMatrix();
		//		mSpotlightWorldViewProjMatrixDirty[index] = false;
		//	}
		//	return mSpotlightWorldViewProjMatrix[index];
		//}
		//else
		//	return BWMatrix4::IDENTITY;
		return BWMatrix4::IDENTITY;
	}
	//-----------------------------------------------------------------------------
	const BWMatrix4& BWAutoParamDataSource::getTextureTransformMatrix(size_t index) const
	{
		// make sure the current pass is set
		assert(mCurrentPass && "current pass is NULL!");
		// check if there is a texture unit with the given index in the current pass
		if (index < mCurrentPass->getNumTextureUnitStates())
		{
			// texture unit existent, return its currently set transform
			return mCurrentPass->getTextureUnitState(static_cast<unsigned short>(index))->getTextureTransform();
		}
		else
		{
			// no such texture unit, return unity
			return BWMatrix4::IDENTITY;
		}
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentRenderTarget(const BWRenderTarget* target)
	{
		mCurrentRenderTarget = target;
	}
	//-----------------------------------------------------------------------------
	const BWRenderTarget* BWAutoParamDataSource::getCurrentRenderTarget(void) const
	{
		return mCurrentRenderTarget;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setCurrentViewport(const BWViewport* viewport)
	{
		mCurrentViewport = viewport;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setShadowDirLightExtrusionDistance(float dist)
	{
		mDirLightExtrusionDistance = dist;
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getShadowExtrusionDistance(void) const
	{
		const BWLight& l = getLight(0); // only ever applies to one light at once
		if (l.getType() == BWLight::LT_DIRECTIONAL)
		{
			// use constant
			return mDirLightExtrusionDistance;
		}
		else
		{
			// Calculate based on object space light distance
			// compared to light attenuation range
			BWVector3 objPos = getInverseWorldMatrix().transformAffine(l.getDerivedPosition(true));
			return l.getAttenuationRange() - objPos.lenth();
		}
	}
	//-----------------------------------------------------------------------------
	const BWRenderable* BWAutoParamDataSource::getCurrentRenderable(void) const
	{
		return mCurrentRenderable;
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseViewProjMatrix(void) const
	{
		return this->getViewProjectionMatrix().inverse();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseTransposeViewProjMatrix(void) const
	{
		return this->getInverseViewProjMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getTransposeViewProjMatrix(void) const
	{
		return this->getViewProjectionMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getTransposeViewMatrix(void) const
	{
		return this->getViewMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseTransposeViewMatrix(void) const
	{
		return this->getInverseViewMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getTransposeProjectionMatrix(void) const
	{
		return this->getProjectionMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseProjectionMatrix(void) const
	{
		return this->getProjectionMatrix().inverse();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseTransposeProjectionMatrix(void) const
	{
		return this->getInverseProjectionMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getTransposeWorldViewProjMatrix(void) const
	{
		return this->getWorldViewProjMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseWorldViewProjMatrix(void) const
	{
		return this->getWorldViewProjMatrix().inverse();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getInverseTransposeWorldViewProjMatrix(void) const
	{
		return this->getInverseWorldViewProjMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getTransposeWorldViewMatrix(void) const
	{
		return this->getWorldViewMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	BWMatrix4 BWAutoParamDataSource::getTransposeWorldMatrix(void) const
	{
		return this->getWorldMatrix().transpose();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTime(void) const
	{
		assert(0);
	//	return ControllerManager::getSingleton().getElapsedTime();
		return 0;
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTime_0_X(float x) const
	{
		return fmod(this->getTime(), x);
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getCosTime_0_X(float x) const
	{
		return cos(this->getTime_0_X(x));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getSinTime_0_X(float x) const
	{
		return sin(this->getTime_0_X(x));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTanTime_0_X(float x) const
	{
		return tan(this->getTime_0_X(x));
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getTime_0_X_packed(float x) const
	{
		float t = this->getTime_0_X(x);
		return BWVector4(t, sin(t), cos(t), tan(t));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTime_0_1(float x) const
	{
		return this->getTime_0_X(x) / x;
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getCosTime_0_1(float x) const
	{
		return cos(this->getTime_0_1(x));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getSinTime_0_1(float x) const
	{
		return sin(this->getTime_0_1(x));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTanTime_0_1(float x) const
	{
		return tan(this->getTime_0_1(x));
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getTime_0_1_packed(float x) const
	{
		float t = this->getTime_0_1(x);
		return BWVector4(t, sin(t), cos(t), tan(t));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTime_0_2Pi(float x) const
	{
		return this->getTime_0_X(x) / x * 2 * PI;
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getCosTime_0_2Pi(float x) const
	{
		return cos(this->getTime_0_2Pi(x));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getSinTime_0_2Pi(float x) const
	{
		return sin(this->getTime_0_2Pi(x));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getTanTime_0_2Pi(float x) const
	{
		return tan(this->getTime_0_2Pi(x));
	}
	//-----------------------------------------------------------------------------
	BWVector4 BWAutoParamDataSource::getTime_0_2Pi_packed(float x) const
	{
		float t = this->getTime_0_2Pi(x);
		return BWVector4(t, sin(t), cos(t), tan(t));
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getFrameTime(void) const
	{
		assert(0);
		return 0;
		//return ControllerManager::getSingleton().getFrameTimeSource()->getValue();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getFPS() const
	{
		return mCurrentRenderTarget->getLastFPS();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getViewportWidth() const
	{
		return static_cast<float>(mCurrentViewport->getActualWidth());
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getViewportHeight() const
	{
		return static_cast<float>(mCurrentViewport->getActualHeight());
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getInverseViewportWidth() const
	{
		return 1.0f / mCurrentViewport->getActualWidth();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getInverseViewportHeight() const
	{
		return 1.0f / mCurrentViewport->getActualHeight();
	}
	//-----------------------------------------------------------------------------
	BWVector3 BWAutoParamDataSource::getViewDirection() const
	{
		return mCurrentCamera->getDerivedDirection();
	}
	//-----------------------------------------------------------------------------
	BWVector3 BWAutoParamDataSource::getViewSideVector() const
	{
		return mCurrentCamera->getDerivedRight();
	}
	//-----------------------------------------------------------------------------
	BWVector3 BWAutoParamDataSource::getViewUpVector() const
	{
		return mCurrentCamera->getDerivedUp();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getFOV() const
	{
		return mCurrentCamera->getFOVy().valueRadians();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getNearClipDistance() const
	{
		return mCurrentCamera->getNearClipDistance();
	}
	//-----------------------------------------------------------------------------
	float BWAutoParamDataSource::getFarClipDistance() const
	{
		return mCurrentCamera->getFarClipDistance();
	}
	//-----------------------------------------------------------------------------
	int BWAutoParamDataSource::getPassNumber(void) const
	{
		return mPassNumber;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::setPassNumber(const int passNumber)
	{
		mPassNumber = passNumber;
	}
	//-----------------------------------------------------------------------------
	void BWAutoParamDataSource::incPassNumber(void)
	{
		++mPassNumber;
	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getSceneDepthRange() const
	{
		static BWVector4 dummy(0, 100000, 100000, 1 / 100000);

		if (mSceneDepthRangeDirty)
		{
			// calculate depth information
			float depthRange = mMainCamBoundsInfo->maxDistanceInFrustum - mMainCamBoundsInfo->minDistanceInFrustum;
			if (depthRange > std::numeric_limits<float>::epsilon())
			{
				mSceneDepthRange = BWVector4(
					mMainCamBoundsInfo->minDistanceInFrustum,
					mMainCamBoundsInfo->maxDistanceInFrustum,
					depthRange,
					1.0f / depthRange);
			}
			else
			{
				mSceneDepthRange = dummy;
			}
			mSceneDepthRangeDirty = false;
		}

		return mSceneDepthRange;

	}
	//-----------------------------------------------------------------------------
	const BWVector4& BWAutoParamDataSource::getShadowSceneDepthRange(size_t index) const
	{
		static BWVector4 dummy(0, 100000, 100000, 1 / 100000);

		if (!mCurrentSceneManager->isShadowTechniqueTextureBased())
			return dummy;

		if (index < OGRE_MAX_SIMULTANEOUS_LIGHTS)
		{
			if (mShadowCamDepthRangesDirty[index] && mCurrentTextureProjector[index])
			{
				const VisibleObjectsBoundsInfo& info =
					mCurrentSceneManager->getVisibleObjectsBoundsInfo(
					(BWCamera*)mCurrentTextureProjector[index]);

				float depthRange = info.maxDistanceInFrustum - info.minDistanceInFrustum;
				if (depthRange > std::numeric_limits<float>::epsilon())
				{
					mShadowCamDepthRanges[index] = BWVector4(
						info.minDistanceInFrustum,
						info.maxDistanceInFrustum,
						depthRange,
						1.0f / depthRange);
				}
				else
				{
					mShadowCamDepthRanges[index] = dummy;
				}

				mShadowCamDepthRangesDirty[index] = false;
			}
			return mShadowCamDepthRanges[index];
		}
		else
			return dummy;
	}
	//---------------------------------------------------------------------
	const ColourValue& BWAutoParamDataSource::getShadowColour() const
	{
		return mCurrentSceneManager->getShadowColour();
	}
	//-------------------------------------------------------------------------
	void BWAutoParamDataSource::updateLightCustomGpuParameter(const BWGpuProgramParameters::AutoConstantEntry& constantEntry, BWGpuProgramParameters *params) const
	{
		unsigned short lightIndex = static_cast<unsigned short>(constantEntry.data & 0xFFFF),
			paramIndex = static_cast<unsigned short>((constantEntry.data >> 16) & 0xFFFF);
		if (mCurrentLightList && lightIndex < mCurrentLightList->size())
		{
			const BWLight &light = getLight(lightIndex);
			light._updateCustomGpuParameter(paramIndex, constantEntry, params);
		}
	}
	void BWAutoParamDataSource::updateActivePass()
	{
		 BWGpuProgramParametersPtr GpuProgramParameter = mCurrentPass->getGPUProgramParameter();
		 SetGPUAutoParameter(GpuProgramParameter);
	}

	void BWAutoParamDataSource::SetGPUAutoParameter(BWGpuProgramParametersPtr GPUProgramParameter)
	{
		if (GPUProgramParameter.Get() != NULL)
		{
			const BWGpuProgramParameters::AutoConstantList &autoConstantList = GPUProgramParameter->GetAutoConstantList();
			BWGpuProgramParameters::AutoConstantList::const_iterator itor = autoConstantList.begin();
			while (itor != autoConstantList.end())
			{
				switch (itor->paramType)
				{
				case BWGpuProgramParameters::ACT_PRE_VIEW_MATIX:
				{
					BWMatrix4 tmp = getPreViewMatrix();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break;
				}
				case BWGpuProgramParameters::ACT_VIEW_MATRIX:
				{
					BWMatrix4 tmp = getViewMatrix();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break;
				}
				case BWGpuProgramParameters::ACT_PRE_WORLD_MATRIX:
				{
					BWMatrix4 tmp = getPreWorldMatrix();  // 这个表示的是 Model Matirx
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break;
				}
				case BWGpuProgramParameters::ACT_WORLD_MATRIX:
				{
					BWMatrix4 tmp = getWorldMatrix();  // 这个表示的是 Model Matirx
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break;
				}
				case BWGpuProgramParameters::ACT_PRE_PROJECTION_MATRIX:
				{
					BWMatrix4 tmp = getPreProjectionMatrix();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break;
				}
				case BWGpuProgramParameters::ACT_PROJECTION_MATRIX:
				{
					BWMatrix4 tmp = getProjectionMatrix();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break;
				}
				case BWGpuProgramParameters::ACT_CAMERA_POSITION:
				{
					BWVector3 Position = GetActiveCamera()->getPosition();
					float M[3];
					M[0] = Position.x;
					M[1] = Position.y;
					M[2] = Position.z;
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, M, 3);
					break;
				}
				case BWGpuProgramParameters::ACT_INVERSE_VIEW_MATRIX:
				{
					BWMatrix4 tmp = getViewMatrix();
					tmp = tmp.inverse();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
					break; 
				}
				case BWGpuProgramParameters::ACT_FOV:
				{
					Radian tmp = GetActiveCamera()->getFOVy();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, &tmp.mRad, 1);
					break;
				}
				case BWGpuProgramParameters::ACT_WIDTH_INVERSE_HIGHT_CLIP:
				{
					float tmp = GetActiveCamera()->GetAspect();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, &tmp, 1);
					break;
				}
				case BWGpuProgramParameters::ACT_FAR_AND_NEAR_CLIP_DISTANCE:
				{
					float NearAndFar[2];
					NearAndFar[0] = GetActiveCamera()->getNearClipDistance();
					NearAndFar[1] = GetActiveCamera()->getFarClipDistance();
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, NearAndFar, 1);
					break;
				}
				case BWGpuProgramParameters::ACT_VIEWPORT_WIDTH_AND_HEIGHT:
				{
					float WidhtAndHeight[2];
					WidhtAndHeight[0] = 1024;
					WidhtAndHeight[1] = 768;
					GPUProgramParameter->WriteRawConstants(itor->physicalIndex, WidhtAndHeight, 2);
					break;
				}
				default:
					break;
				}
				itor++;
			}
		}
	}

