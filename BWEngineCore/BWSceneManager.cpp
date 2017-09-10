#include "BWSceneManager.h"
#include "BWRoot.h"
#include "BWAutoParamDataSource.h"
#include "BWSceneNode.h"
#include "BWCamera.h"
#include "BWRenderOperation.h"
#include "BWMaterialManager.h"
#include "BWPointLight.h"
#include"BWDirectionalLight.h"
#include"BWLog.h"
BWSceneManagerRenderableVisitor::BWSceneManagerRenderableVisitor() :manualLightList(NULL)
, mTargetSceneManager(NULL)
, mUsePass(NULL)
, autoLight(false)
, scissoring(false)
{
	
}
BWSceneManagerRenderableVisitor::~BWSceneManagerRenderableVisitor()
{

}
bool BWSceneManagerRenderableVisitor::visitor(const BWPass *pass)
{
	if (!mTargetSceneManager->validatePassForRendering(pass)) // �ó����л������������Ⱦ
	{
		return false;
	}
	mUsePass = mTargetSceneManager->setPass(pass);
	return true;
}
void BWSceneManagerRenderableVisitor::visitor( BWRenderable *renderable)
{
    if (!mTargetSceneManager->validateRenderableForRendering(mUsePass , renderable))
    {
		mTargetSceneManager->renderSingleObject(renderable, mUsePass, scissoring, autoLight, manualLightList);
    }
}
void BWSceneManagerRenderableVisitor::visitor(RenderablePass *renderablePass)
{
	return;
}

BWSceneManager::BWSceneManager() :
mActiveCamera(NULL), 
mActiveRenderSystem(NULL),
mRootSceneNode(NULL),
mFindVisibleObject(true),
mSuppressRenderStateChanges(false),
mFlipCullingOnNegativeScale(false),
mCameraRelativeRendering(false),
mGpuParamsDirty(0),
mNormaliseNormalsOnScale(false)
{
	mAutoParamDataSource = new BWAutoParamDataSource();
	renderQueue = new BWRenderQueue();
	mAcitiveSceneManagerRenderableVisitor = new BWSceneManagerRenderableVisitor();
	mAcitiveSceneManagerRenderableVisitor->setTargetSceneManager(this);
}
BWSceneManager::~BWSceneManager()
{

}
void BWSceneManager::_renderScene(BWCamera *camera, BWViewport *viewport, bool isShowOverlay)
{
	mActiveCamera = camera;
	mActiveRenderSystem = BWRoot::GetInstance()->getRenderSystem();
	mActiveRenderSystem->SetGlobalDataSet(mAutoParamDataSource);
	mAutoParamDataSource->setCurrentSceneManager(this);
	mCurrentViewport = viewport;
	 // ������һ���趨 ֻҪ���������³����е����ݵ� ���� ������

	 // �ж��Ƿ������Ӱ

	// ���³�������
	_updateSceneGraph(mActiveCamera);

	AutoTrackingSceneNodes::iterator itor = mAutoTrackingSceneNodes.begin();
	AutoTrackingSceneNodes::iterator itorEnd = mAutoTrackingSceneNodes.end();
	while (itor != itorEnd)
	{
		(*itor)->_autoTrack();
		itor++;
	}
	mActiveCamera->_autoTrack();

	mAutoParamDataSource->setCurrentViewport(mCurrentViewport);
	setViewport(viewport);

	mAutoParamDataSource->setCurrentCamera(mActiveCamera, mCameraRelativeRendering);
	mAutoParamDataSource->setShadowDirLightExtrusionDistance(mShadowDirLightExtrudeDist);

	mAutoParamDataSource->setAmbientLightColour(mAmbientLight);
	// ΪʲôҪ��дһ��
	mActiveRenderSystem->setAmbientLight(mAmbientLight.r, mAmbientLight.g, mAmbientLight.b);
	mAutoParamDataSource->setCurrentRenderTarget(viewport->getRenderTarget());
	prepareRenderQueue();

	if (mFindVisibleObject) // ���в���Ҫ��Ⱦ��ô��  �Ҳ¿����Ƕ�һ��pass��Ⱦ�ö���ʱ��
	{
		CamVisibleObjectsMap::iterator camVisObject = mCamVisibleObjectMap.find(camera);
		assert(camVisObject != mCamVisibleObjectMap.end());
		camVisObject->second.reset();
		firepreFindVisibleObject(viewport);
		_findVisibleObjects(camera, &(camVisObject->second), mIlluminationStage == IRS_RENDER_TO_TEXTURE ? true : false);
		firePostFindVisibleObject(viewport);
	}
	mActiveRenderSystem->_beginGeometryCount();
	if (viewport->getClearEveryFrame())
	{
		mActiveRenderSystem->clearFrameBuffer(viewport->getClearFrameBuffer(), viewport->getBackGroundColour());
	}
	mAutoParamDataSource->SetActiveLightMap(GetActiveDirectionalLights());
	mAutoParamDataSource->SetObejctesQueueInLight(GetObjectQueueWithinLightMap());

	mActiveRenderSystem->_beginFrame();
	mActiveRenderSystem->_setPolygonMode(camera->getPolygonMode());
	mActiveRenderSystem->_setProjectionMatrix(camera->getProjectionMatrixRS());
	mCachedViewMatrix = camera->getViewMatrix(true);

	if (mCameraRelativeRendering)
	{
		mCachedViewMatrix.setTrans(BWVector3::ZERO);
		mCameraRelativePosition = camera->getDerivedDirection();
	}
	mActiveRenderSystem->_setTextureProjectionRelativeTo(mCameraRelativeRendering, camera->getDerivedPosition());
	setViewMatrix(mCachedViewMatrix);
	
	_renderVisibleObjec();

	mActiveRenderSystem->_endFrame();
	camera->_notifyRenderedFaces(mActiveRenderSystem->_getFaceNum());
	camera->_notifyRenderedBathces(mActiveRenderSystem->_getBathcesNum());
}
void BWSceneManager::_renderVisibleObjec()
{
	//assert(0);  ���������viewport��������Ⱦ���еķ���˳��  ����Ŀǰ��ʱʹ��Ĭ�Ϸ���˳��
	//if����������Ⱦ˳��
	//{
	//���ն��Ƶ���Ⱦ˳��Ⱦ
    //}
	//else
	{
		renderVisibleObjectsDefaultsSequence();
	}
}
void BWSceneManager::renderVisibleObjectsDefaultsSequence()
{
	firePreRenderQueue();
	RenderQueueGroupList::iterator itor = getRenderQueue()->getRenderQueueGroupList().begin();
	RenderQueueGroupList::iterator endItor = getRenderQueue()->getRenderQueueGroupList().end();
	while (itor != endItor)
	{
		unsigned char renderGroupID = itor->first;
		if (!isRenderQueueGroupToBeProcessed(renderGroupID))
		{
			continue;
		}
		BWRenderQueueGroup* renderQueueGroup = itor->second;
		bool repeatQueue = false;
	    do 
	    {
			if (!firePreRenderQueueGroupStarted(renderGroupID))
			{
				continue;
			}
			_renderQueueGroupObject(renderQueueGroup, BWQueueRenderableCollection::OM_PASS_GROUP);
			if (!firePostRenderQueueGroupEnded(renderGroupID))
			{
				repeatQueue = true;
			}
			else
			{
				repeatQueue = false;
			}
	    } while (repeatQueue);	
		itor++;
	}
	firePostRenderQueue();
}
void BWSceneManager::_renderQueueGroupObject(BWRenderQueueGroup* renderQueueGroup, BWQueueRenderableCollection::OrganisationMode organisationMode)
{
	//���ﻹҪ������Ӱ��  ������ʱ�û�����Ⱦ

	renderBasicQueueGroupObject(renderQueueGroup, organisationMode);
}
bool BWSceneManager::validatePassForRendering(const BWPass *pass)
{
	return true;
}
bool BWSceneManager::firePreRenderQueueGroupStarted(unsigned char renderGroupID)
{
	return true;
}
bool BWSceneManager::firePostRenderQueueGroupEnded(unsigned char renderGroupID)
{
	return true;
}
bool BWSceneManager::isRenderQueueGroupToBeProcessed(unsigned char goupID)
{
	return true;
}
bool BWSceneManager::validateRenderableForRendering(const BWPass *pass, const BWRenderable *renderable)
{
	return false;
}
void BWSceneManager::renderBasicQueueGroupObject(BWRenderQueueGroup* renderQueueGroup, BWQueueRenderableCollection::OrganisationMode organisationMode)
{
	RenderPriorityGroupList::iterator itor = renderQueueGroup->getRenderPriorityGroupList().begin();
	RenderPriorityGroupList::iterator endItor = renderQueueGroup->getRenderPriorityGroupList().end();
	while (itor != endItor)
	{
		renderObject(itor->second->getSolidsBasic(), organisationMode, true, true);
		renderObject(itor->second->getTransparent(), organisationMode, true, true);
		itor++;
	}
}
const BWPass* BWSceneManager::setPass(const BWPass *pass)
{
	mAutoParamDataSource->setCurrentPass(pass);
	bool passSurfaceAndLiaghtParams = false; // gpu�����Ƿ��еƹ�������ز������뵽rendersystem�� Ŀǰû��GPU����
	bool passFogParams =false;               // ��Ĳ����Ƿ���GPU��
	if (pass->hasVertexProgram())
	{
		mActiveRenderSystem->bindGpuProgram(pass->GetVertexProgram().Get());
		passSurfaceAndLiaghtParams = pass->GetVertexProgram()->getPassSurfaceAndLightStates();
	}
	else
	{
		if (mActiveRenderSystem->isGpuProgramBound(GPT_VERTEX_PROGRAM))
		{
			mActiveRenderSystem->unbindGpuProgram(GPT_VERTEX_PROGRAM);
		}
	}
	// ��������geometryshader

	if (passSurfaceAndLiaghtParams)
	{
		assert(0);// ���ｫGPU���ݴ���
	}
	
	if (pass->hasFragmentProgram())
	{
		mActiveRenderSystem->bindGpuProgram(pass->GetFragmetProgram().Get());
		passFogParams = pass->GetFragmetProgram()->getPassFogStates();
	}
	else
	{
		if (mActiveRenderSystem->isGpuProgramBound(GPT_FRAGMENT_PROGRAM))
		{
			mActiveRenderSystem->unbindGpuProgram(GPT_FRAGMENT_PROGRAM);
		}
	}
	if (passFogParams)
	{
		assert(0);//�������ø����� ��ص�����
	}
	 //mAutoParamDataSource->setFog() ��ֻ����ص�����

	// ���ó������ Ҳ���Ǳ���Ⱦ����Ҫ��������г������
	if (pass->hasSeparateSceneBlending()) // Դ������Ӻ�Ŀ�Ļ�������Ƿ�ֿ�
	{
		mActiveRenderSystem->_setSeparatesceneBlending(
			pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
			pass->getSourceBlendFactorAlpha(), pass->getDestBlendFactorAlpha(),
			pass->getSceneBlendingOperation(),
			pass->hasSeparateSceneBlendingOperation() ? pass->getSceneBlendingOperation() : pass->getSceneBlendingOperationAlpha()
			);
	}
	else
	{
		if (pass->hasSeparateSceneBlendingOperation())
		{
			mActiveRenderSystem->_setSeparatesceneBlending(
				pass->getSourceBlendFactor(), pass->getDestBlendFactor(),
				pass->getSourceBlendFactorAlpha(), pass->getDestBlendFactorAlpha(),
				pass->getSceneBlendingOperation(),
				pass->getSceneBlendingOperation()
				);
		}
		else
		{
			mActiveRenderSystem->_setSceneBlending(pass->getSourceBlendFactor(), pass->getDestBlendFactor(), pass->getSceneBlendingOperation());
		}
	}

	// ���õ����ز���
	mActiveRenderSystem->_setPointParameters(
		pass->getPointSize(),
		pass->isPointAttenuationEnabled(),
		pass->getPointAttenuationConstant(),
		pass->getPointAttenuationLinear(),
		pass->getPointAttenuationQuadratic(),
		pass->getPointMinSize(),
		pass->getPointMaxSize()
		);
	//����Ҫ����rendersystem �Ƿ�֧�� point sprite

	size_t textureNum = pass->getNumTextureUnitStates();
	size_t startLigthIndex = pass->getStartLight();
	for (int i = 0; i < textureNum; i ++)
	{
		BWTextureUnitState *textureUnitState = pass->getTextureUnitState(i);
		if (mIlluminationStage == IRS_NONE && pass->hasVertexProgram())
		{
			BWTextureUnitState::EffectMap::const_iterator itor =
				textureUnitState->getEffects().find(BWTextureUnitState::ET_PROJECTIVE_TEXTURE);
			if (itor != textureUnitState->getEffects().end())
			{
				mAutoParamDataSource->setTextureProjector(itor->second.frustum, i);
			}
		}
		if (textureUnitState->getContentType() == BWTextureUnitState::CONTENT_COMPOSITOR) // ������Ч �Ժ���д
		{
			assert(0);
		}
		mActiveRenderSystem->_setTextureUnitSetting(i, *textureUnitState);
	}
	mActiveRenderSystem->_disableTextureUnitsFrom(pass->getNumTextureUnitStates());

	mActiveRenderSystem->_setDepthBufferFunction(pass->getDepthFunction());
	mActiveRenderSystem->_setDepthBufferCheckEnable(pass->getDepthCheckEnable());
	mActiveRenderSystem->_setDepthBufferWriteEnable(pass->getDepthWriteEnable());
	mActiveRenderSystem->_setDepthBias(pass->getDepthBiasConstant(),
		pass->getDepthBiasSlopScale());
	mActiveRenderSystem->_setAlphaRejectSetting(
		pass->getAlphaRejectFuntion(), pass->getAlphaRejectValue(), pass->isAlphaToCoverageEnable());

	bool colWrite = pass->getColourWriteEnable();
	mActiveRenderSystem->_setColourBufferEnable(colWrite, colWrite, colWrite, colWrite);

	mActiveRenderSystem->_setCullingMode(pass->getCullingMode());
	
	mActiveRenderSystem->setShadingType(pass->getShadingMode());

	mActiveRenderSystem->_setPolygonMode(pass->getPolygonMode());

	mAutoParamDataSource->setPassNumber(pass->getIndex());

	return pass;
}
void BWSceneManager::useRenderableViewProjMode(const BWRenderable *renderable, bool fixedFuntion)
{
	bool useIdentityView = renderable->getUseIdentityView();
	if (useIdentityView)
	{
		if (fixedFuntion)
		{
			setViewMatrix(BWMatrix4::IDENTITY);
		}
		mGpuParamsDirty |= (unsigned short)GPV_GLOBAL;
		mResetIdentityView = true;
	}
	bool useIdentityProj = renderable->getUseIdentityProjection();
	if (useIdentityProj)
	{
		if (fixedFuntion)
		{
			BWMatrix4 matrix;
			mActiveRenderSystem->_convertProjectionMatrix(BWMatrix4::IDENTITY, matrix);
			mActiveRenderSystem->_setProjectionMatrix(matrix);
		}
		mGpuParamsDirty |= (unsigned short)GPV_GLOBAL;
		mResetIdentityProj = true;
	}
}
void BWSceneManager::resetViewProjMode(bool fixedFunction)
{
	if (mResetIdentityView)
	{
		if (fixedFunction)
		{
			setViewMatrix(mCachedViewMatrix);
		}
		mGpuParamsDirty |= (unsigned short)GPV_GLOBAL;

		mResetIdentityView = false;
	}
	if (mResetIdentityProj)
	{
		if (fixedFunction)
		{
			mActiveRenderSystem->_setProjectionMatrix(mActiveCamera->getProjectionMatrixRS());
		}
		mGpuParamsDirty |= (unsigned short)GPV_GLOBAL;
		mResetIdentityProj = false;
	}
}
void BWSceneManager::renderSingleObject(  BWRenderable *renderable, const BWPass *pass, bool scissoring, bool autoLight, const LightList* manualLightList)
{
	unsigned short numMatrices;
	BWRenderOperation ro;
	const_cast<BWRenderable*>(renderable)->getRenderOperation(ro);
	ro.sourceRenderable = renderable;

	BWGpuProgram *vGpuProgram = pass->hasVertexProgram() ? pass->GetVertexProgram().Get() : NULL;
	bool passTransformState = true;  // �ñ�����ʾ�����Ƿ���ģ�;���

	if (vGpuProgram)
	{
		passTransformState = vGpuProgram->getPassTransformStates();
	}
	numMatrices = renderable->getNumWorldTransforms();

	if (numMatrices > 0)
	{
		renderable->getWorldTransforms(mTempXform);
		if (mCameraRelativeRendering && !renderable->getUseIdentityView())// ��ʲô�õ�  ��ȾUI������
		{
			for (int i = 0; i < numMatrices; i++)
			{
				mTempXform[i].setTrans(mTempXform[i].getTrans() - mCameraRelativePosition);
			}
		}
	   if (passTransformState)
	   {
		   if (numMatrices > 1)
		   {
			   mActiveRenderSystem->_setWorldMatrices(mTempXform, numMatrices);
		   }
		   else
		   {
			 /*����worldmatrix
			  mAutoParamDataSource->setCurrentRenderable(renderable);
			   BWMatrix4 tmp = mAutoParamDataSource->getWorldMatrix();
			   mActiveRenderSystem->_setWorldMatrix(tmp);
			    */
			   mActiveRenderSystem->_setWorldMatrix(*mTempXform);
		   }
	   }
	}
	useRenderableViewProjMode(renderable, passTransformState);

	mGpuParamsDirty |= (unsigned short)GPV_PER_OBJECT;

	if (!mSuppressRenderStateChanges)
	{
		bool passSurfaceAndLightParam = true;
		if (pass->isProgrammable())
		{
			mAutoParamDataSource->setCurrentRenderable(renderable);
			mAutoParamDataSource->setWorldMatrices(mTempXform, numMatrices);
			if (vGpuProgram)
			{
				passSurfaceAndLightParam = vGpuProgram->getPassSurfaceAndLightStates();
			}
		}
		for (int i = 0; i < pass->getNumTextureUnitStates();i++) // �����������ú��������ص�����ӳ�䷽ʽ
		{
			BWTextureUnitState *texture = pass->getTextureUnitState(i);
			if (texture->hasViewRelativeTextureCoordinateGeneration())  //��ʱ������false
			{
				mActiveRenderSystem->_setTextureUnitSetting(i, *texture);
			}
		}

		if((pass->getNormaliseNormals() || mNormaliseNormalsOnScale) && mTempXform[0].hasScale())
		{
			mActiveRenderSystem->setNormaliseNormals(true);
		}
		else
		{
			mActiveRenderSystem->setNormaliseNormals(false);
		}

		if (mFlipCullingOnNegativeScale)
		{
			CullingMode cullMode = mPassCullingMode;
			if (mTempXform[0].hasNegativeScale())
			{
				switch (cullMode)
				{
				case CULL_NONE:
					break;
				case CULL_CLOCKWISE:
					cullMode = CULL_ANTICLOCKWISE;
					break;
				case CULL_ANTICLOCKWISE:
					cullMode = CULL_CLOCKWISE;
					break;
				default:
					break;
				}
			}
			if (cullMode != mActiveRenderSystem->_getCullingMode())
			{
				mActiveRenderSystem->_setCullingMode(cullMode);
			}

		}
		PolygonMode polygonMode = pass->getPolygonMode();
		if (pass->getPolygonModeOverrideable() && renderable->getPolygonModeOverrideable())
		{
			PolygonMode camPolyMode = mActiveCamera->getPolygonMode();
			if (camPolyMode < polygonMode)
			{
				polygonMode = camPolyMode;
			}
		}
		mActiveRenderSystem->_setPolygonMode(polygonMode);
		if (autoLight)
	    {
			//assert(0);
			//static LightList localLightList;
			//const LightList &rendLightList = renderable->getLights();
			//bool iteratorPreLight = pass->getIteratorPreLight();

			//int lightLeft = 1;
			//if (iteratorPreLight)
			//{
			//	lightLeft = static_cast<int> (rendLightList.size() - pass->getStartLight());
			//	if (lightLeft > static_cast<int>(pass->getMaxSimultaneousLights()))
			//	{
			//		lightLeft = static_cast<int>(pass->getMaxSimultaneousLights());
			//	}
			//}

			//const LightList *pLightListToUse;
			//size_t  lightIndxe = pass->getStartLight();
			//size_t depthInc = 0;
			//if (lightLeft > 0)
			//{
			//	if (iteratorPreLight)
			//	{
			//		size_t shadow
			//	}
			//	assert(0);  // �漰�����պ���Ӱ
			//}
	    }
		else
		{
			//assert(0); 
			/*bool skipBecaseOfLightType = false;
			if (pass->getRunOnlyForOneLigthType())
			{
				if (!manualLightList ||
					(manualLightList->size() == 1 &&
					manualLightList->at(0)->getType() != pass->getOnlyLightType())
					)
				{
					skipBecaseOfLightType = true;
				}
			}

			if (!skipBecaseOfLightType)
			{
				fireRenderSingleObject(renderable, pass, mAutoParamDataSource, manualLightList, mSuppressRenderStateChanges);
				if (pass->isProgrammable())
				{
					if (manualLightList)
					{
						useLightsGpuProgram(pass, manualLightList);
					}
				}
				if (manualLightList && pass->getLightingEnable() && passSurfaceAndLightParam)
				{
					useLights(*manualLightList, pass->getMaxSimultaneousLights());
				}
				ClipResult scissored = CLIPPED_NONE;
				ClipResult clipped = CLIPPED_NONE;
				if (scissoring && manualLightList && pass->getLightScissoringEnable())
				{
					scissored = buildAndSetScissor(*manualLightList, mActiveCamera);
				}
				if (scissoring && manualLightList && pass->getLightScissoringEnable())
				{
					clipped = buildAndSetLightClip(*manualLightList);
				}

				if (scissored != CLIPPED_ALL && clipped != CLIPPED_ALL)
				{
					mActiveRenderSystem->setCurrentPassIterationCount(pass->getPassIterationCount());
					updateGpuProgramParameters(pass);
					if (renderable->preRender(this , mActiveRenderSystem))
					{
						mActiveRenderSystem->_render(ro);
					}
					renderable->postRender(this, mActiveRenderSystem);
				}
				if (scissored ==  CLIPPED_SOME)
				{
					resetScissor();
				}
				if (clipped == CLIPPED_SOME)
				{
					resetLightClip();
				}
			}
			*/
		}

		mAutoParamDataSource->setCurrentRenderable(renderable); // ��ʱ���
		// �������ø���pass��GPUProgram�еı���
		mAutoParamDataSource->updateActivePass();
		fireRenderSingleObject(renderable, pass, mAutoParamDataSource, NULL, mSuppressRenderStateChanges);
		if (pass->isProgrammable())
		{
			RSGraphicPipelineState Pipeline;
			Pipeline.GPUProgramUsage = pass->getGPUProgramUsage();
			mActiveRenderSystem->SetGraphicsPipelineState(Pipeline);
			mActiveRenderSystem->SetupGBufferRenderTarget(pass->getGPUProgramUsage());
		}

		mActiveRenderSystem->setCurrentPassIterationCount(1);
		if (renderable->preRender(this, mActiveRenderSystem))
		{
			mActiveRenderSystem->_render(ro);
		}
		renderable->postRender(this, mActiveRenderSystem);
	}
	else
	{
		fireRenderSingleObject(renderable, pass, mAutoParamDataSource, NULL, mSuppressRenderStateChanges);
		mActiveRenderSystem->setCurrentPassIterationCount(1);
		if (renderable->preRender(this , mActiveRenderSystem))
		{
			mActiveRenderSystem->_render(ro);
		}
		renderable->postRender(this, mActiveRenderSystem);
	}
	resetViewProjMode(passTransformState);
}
bool BWSceneManager::fireRenderSingleObject(const BWRenderable *renderable, const BWPass *pass, const BWAutoParamDataSource *autoParamDataSource, const LightList* manualLightList, bool suppressRenderStateChanges)
{
	// ͨ��ע���listener ����Ӧ�¼�
	//�ڸĺ����п��Զ�Ҫ��Ⱦ����ò�����������
	return true;
}
void BWSceneManager::renderObject(BWQueueRenderableCollection& renderableCollection, BWQueueRenderableCollection::OrganisationMode OM, bool lightScissoringClipping, bool doLightItertion, const LightList* manualLightList /* = NULL */)
{
	mAcitiveSceneManagerRenderableVisitor->autoLight = false; //doLightItertion;
	mAcitiveSceneManagerRenderableVisitor->manualLightList = NULL;//manualLightList;
	mAcitiveSceneManagerRenderableVisitor->scissoring = false; //lightScissoringClipping;

	renderableCollection.accept(*mAcitiveSceneManagerRenderableVisitor, OM);
}
BWSceneNode * BWSceneManager::getRootSceneNode()
{
	if (!mRootSceneNode)
	{
		mRootSceneNode = createSceneNode();
	}
	return mRootSceneNode;
}
void BWSceneManager::_findVisibleObjects(BWFrustum *camera, VisibleObjectsBoundsInfo *visibelBoundInfor, bool isrenderToTexture)
{
     getRootSceneNode()->_findVisibleObject(camera, renderQueue, visibelBoundInfor);
	 //�����ʹ�õƹ��renderQueue������ShadowMap��
	 ObjectsQueneWithInLightMap::iterator LightItor = ObjectsQuenesWithInLight.begin();
	 while (LightItor != ObjectsQuenesWithInLight.end())
	 {
		 BWLight* light = mActiveLightList[LightItor->first];
		 getRootSceneNode()->_findVisibleObject(light, LightItor->second, visibelBoundInfor);
		 LightItor++;
	 }
}
BWSceneNode* BWSceneManager::createSceneNode()
{
	BWSceneNode *sn = createSceneNodeImp();
	mSceneNodeList.push_back(sn);
	return sn;
}
BWSceneNode* BWSceneManager::createSceneNodeImp(const std::string &name)
{
	return new BWSceneNode(this);
}
BWSceneNode* BWSceneManager::createSceneNodeImp()
{
	return new BWSceneNode(this);
}
void BWSceneManager::_updateSceneGraph(BWCamera *camera)
{
	getRootSceneNode()->update(true, false);
	DirectionalLightMap& Lights = GetActiveDirectionalLights();
	DirectionalLightMap::iterator itor = Lights.begin();
	while (itor != Lights.end())
	{
		BWDirectionalLight* Light = itor->second;
		Light->UpdateProjectedShadowMap(camera);
		itor++;
	}
}

bool BWSceneManager::getShowBoundingBox()
{
	return false;
}
bool BWSceneManager::isShadowTechniqueTextureBased() const
{
	return true;
}
const ColourValue& BWSceneManager::getShadowColour() const
{
	return ColourValue::Black;
}
const VisibleObjectsBoundsInfo& BWSceneManager::getVisibleObjectsBoundsInfo(BWCamera * camera) const
{
	VisibleObjectsBoundsInfo tmp;
	return tmp;
}

void BWSceneManager::firePostFindVisibleObject(BWViewport *viewPort) 
{

}
void BWSceneManager::firepreFindVisibleObject(BWViewport* viewport) 
{

}
void BWSceneManager::prepareRenderQueue()
{
	// ��������Ⱦ����
	renderQueue->clear();
	//���������Ⱦ����
	ObjectsQueneWithInLightMap::iterator itor = ObjectsQuenesWithInLight.begin();
	while (itor != ObjectsQuenesWithInLight.end())
	{
		itor->second->clear();
		itor++;
	}
}
void BWSceneManager::setViewMatrix(const BWMatrix4 &matrix)
{
	mActiveRenderSystem->_setViewMatrix(matrix);
	// ���ﻹ�й������
}
void BWSceneManager::setViewport(BWViewport *viewport)
{
	mCurrentViewport = viewport;
	mActiveRenderSystem->_setViewport(viewport);
	BWMaterialManager::GetInstance()->setActiveScheme(viewport->getMaterialScheme());
}
BWRenderQueue*  BWSceneManager::getRenderQueue()
{
	return renderQueue;
}
void VisibleObjectsBoundsInfo::reset()
{

}
void BWSceneManager::firePostRenderQueue()
{

}
void BWSceneManager::firePreRenderQueue()
{

}
BWDirectionalLight* BWSceneManager::CreateDirectionalLight(const std::string &name)
{
	return dynamic_cast<BWDirectionalLight*>(CreateLight(name, BWLight::LightType::LT_DIRECTIONAL));
}
BWPointLight* BWSceneManager::CreatePointLight(const std::string &name)
{
	return dynamic_cast<BWPointLight*>(CreateLight(name, BWLight::LightType::LT_POINT));
}

BWRenderQueue * BWSceneManager::GetObjectsQueueWithinLight(const std::string & LgihtName) 
{
	
	return nullptr;
}
bool BWSceneManager::CreateObjectsQueueWithinLight(const std::string & name)
{
	ObjectsQueneWithInLightMap::iterator ObjectsQueueItor = ObjectsQuenesWithInLight.find(name);
	if (ObjectsQueueItor != ObjectsQuenesWithInLight.end())
	{
		assert(0);
	}
	ObjectsQuenesWithInLight[name] = new BWRenderQueue();
	return  true;
}
void BWSceneManager::AddDirectLight(BWDirectionalLight * DirectionalLight)
{
	if (DirectionalLight)
	{
		if (DirectionalLight->getSceneManager() != this)
		{
			Log::GetInstance()->logMessage("BWSceneManager::addActiveLight��can not add lightsource ");
			return;
		}
		else
		{
			ActiveDirectionalLightMap[DirectionalLight->getName()] = DirectionalLight;
			mActiveLightList[DirectionalLight->getName()] = DirectionalLight;
		}
	}
}
void BWSceneManager::AddPointLight(BWPointLight * PointLight)
{
	if (PointLight)
	{
		if (PointLight->getSceneManager() != this)
		{
			Log::GetInstance()->logMessage("BWSceneManager::addActiveLight��can not add lightsource ");
			return;
		}
		else
		{
			ActivePointLihgtMap[PointLight->getName()] = PointLight;
			mActiveLightList[PointLight->getName()] = PointLight;
		}
	}
	
}
BWLight* BWSceneManager::CreateLight(const std::string &name , BWLight::LightType LightType)
{
	LightList::iterator  itor = mAllLight.find(name);
	if (itor != mAllLight.end())
	{
		assert(0);
	}
	BWLight* light = NULL;
	switch (LightType)
	{
	case BWLight::LightType::LT_DIRECTIONAL:
		light = new BWDirectionalLight(name, this);
		break;
	case BWLight::LightType::LT_POINT:
		light = new BWPointLight(name, this);
		break;
	case BWLight::LightType::LT_SPOTLIGHT:
		break;
	default:
		break;
	}
	mAllLight[name] = light;
	CreateObjectsQueueWithinLight(name);
	return light;
}
BWCamera* BWSceneManager::createCamera(const std::string &name)
{
	CameraMap::iterator  itor = mCameraMap.find(name);
	if (itor != mCameraMap.end())
	{
		assert(0);
	}
	BWCamera *tmp = new BWCamera(this, name);

	mCameraMap[name] = tmp;
	mCamVisibleObjectMap[tmp] = VisibleObjectsBoundsInfo();
	return tmp;
}