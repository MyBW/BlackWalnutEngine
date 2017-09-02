#include "BWRenderSystem.h"
#include "BWCommon.h"
#include "BWVertexIndexData.h"
#include"BWMeshManager.h"
#include"BWMaterialManager.h"
#include "BWLog.h"
#include"BWRoot.h"
#include"BWSceneManager.h"
#include"BWRenderQueue.h"
#include"BWAutoParamDataSource.h"
#include <assert.h>
#include <stdlib.h>
BWRenderSystem* BWRenderSystem::instance = NULL;
static const BWTexturePtr sNullTexPtr;
BWRenderSystem::BWRenderSystem():mDisableTexUnitFrom(0)
,mInvertVertexWinding(false)
{
	
}
BWRenderSystem::~BWRenderSystem()
{

}
BWRenderSystem * BWRenderSystem::GetInstance()
{
	assert(instance);
	return instance;
}
bool BWRenderSystem::isGpuProgramBound(GpuProgramType GPT)
{
	switch (GPT)
	{
	case GPT_VERTEX_PROGRAM:
		return mVertexProgramBound;
	case GPT_FRAGMENT_PROGRAM:
		return mFragmentProgramBound;
	case GPT_GEOMETRY_PROGRAM:
		return mGeometryProgramBound;
	}
}
void BWRenderSystem::_disableTextureUnitsFrom(size_t texUnit)
{
	size_t disableTo = OGRE_MAX_TEXTURE_LAYERS;
	if (disableTo > mDisableTexUnitFrom)
	{
		disableTo = mDisableTexUnitFrom;
	}
	mDisableTexUnitFrom = texUnit;
	for (size_t i = texUnit; i < disableTo; i++)
	{
		_disableTextureUnit(i);
	}
}
void BWRenderSystem::_setVertexTexture(size_t texUnit, const BWTexturePtr &texture)
{
	assert(0);
}
void BWRenderSystem::_disableTextureUnit(size_t texUnit)
{
	_setTexture(texUnit, false, sNullTexPtr);
}

void BWRenderSystem::_initRenderTargets()
{
	RenderTargetMap::iterator itor = mRenderTargetMap.begin();
	while (itor != mRenderTargetMap.end())
	{
		itor->second->resetStatistics();
		itor++;
	}
}
void BWRenderSystem::_updateAllRenderTargets(bool swapBuffer)
{
	// 这里更新的时候根据优先级跟新 保证先得到render to texture  然后才是render to window
	RenderTargetPriorityMap::iterator itor = mPrioritiseRenderTargets.begin();
	RenderTargetPriorityMap::iterator itorEnd = mPrioritiseRenderTargets.end();
	for (; itor != itorEnd; itor++)
	{
		if (itor->second->isActive() && itor->second->isAutoUpdated())
		{
			itor->second->update(swapBuffer);
		}
	}
}
void BWRenderSystem::setWaitForVerticalBlank(bool enable)
{
	mVSync = enable;
}
bool BWRenderSystem::updatePassIterationRenderState()
{
	if (mCurrentPassIterationCount <= 1)
	{
		return false;
	}
	--mCurrentPassIterationCount;
	++mCurrentPassIterationNum;
	// 这里设置各种shader 
	return true;
}
void BWRenderSystem::bindGpuProgram(BWGpuProgram *GpuProgamr)
{
	assert(0);
}
bool BWRenderSystem::getWaitForVerticalBlank()
{
	return true;
}
void BWRenderSystem::attacheRenderTarget(BWRenderTarget *target)
{
	assert(target->getPriority() < OGRE_NUM_RENDERTARGET_GROUPS);
	mRenderTargetMap.insert(RenderTargetMap::value_type(target->getName(), target));
	mPrioritiseRenderTargets.insert(RenderTargetPriorityMap::value_type(target->getPriority(), target));
}
void BWRenderSystem::_swapAllRenderTargetBuffers(bool waitForVSync)
{
	RenderTargetPriorityMap::iterator itor = mPrioritiseRenderTargets.begin();
	RenderTargetPriorityMap::iterator itorEnd = mPrioritiseRenderTargets.end();
	for (; itor != itorEnd; itor++)
	{
		if (itor->second->isActive() && itor->second->isAutoUpdated())
		{
			itor->second->swapBuffers(waitForVSync);
		}
	}
}

unsigned int BWRenderSystem::_getBathcesNum()
{
	return 0;
}
unsigned int BWRenderSystem::_getFaceNum()
{
	return 0;
}

void BWRenderSystem::_beginFrame()
{
	
}

void BWRenderSystem::_beginGeometryCount()
{

}
CullingMode BWRenderSystem::_getCullingMode()  const 
{
	return mCullingMode;
}

void BWRenderSystem::_setTextureProjectionRelativeTo(bool camraRelativeRendering, BWVector3 pos)
{

}
void BWRenderSystem::_endFrame()
{
	//延迟渲染相关  

	RenderAmbientOcclusion();
	//光照相关
	RenderLights();
	RenderInDirectLights();
	//遮挡相关

	//天空盒
	RenderSkyBox();

	GRenderTarget->CopyToScreenFromColorBuffer("FinalRenderResult");
	GRenderTarget->clearRenderTarget();
}

void BWRenderSystem::setAmbientLight(float r, float g, float b)
{

}
void BWRenderSystem::_setTextureUnitFiltering(size_t texUnit, FilterOptions min, FilterOptions mag, FilterOptions mip)
{
	_setTextureUnitFiltering(texUnit, FT_MIN, min);
	_setTextureUnitFiltering(texUnit, FT_MAG, mag);
	_setTextureUnitFiltering(texUnit, FT_MIP, mip);
}
void BWRenderSystem::_setTextureUnitSetting(size_t texUnit, BWTextureUnitState& textureUnit)
{
	const BWTexturePtr &texture = textureUnit._getTexturePtr();
	// 这里还有render system 能力测试
	if (false) //  opengl本身不支持纹理的一些操作
	{
		if (textureUnit.getBindingType() == BWTextureUnitState::BT_VERTEX)
		{
			_setVertexTexture(texUnit, texture);
			_setTexture(texUnit, true, sNullTexPtr);
		}
		else
		{
			_setVertexTexture(texUnit, sNullTexPtr);
			_setTexture(texUnit, true, texture);
		}
	}
	else
	{
		_setTexture(texUnit, true, texture);
	}

	_setTextureCoordSet(texUnit, textureUnit.getTextureCoordSet());

	_setTextureUnitFiltering(texUnit,
		textureUnit.getTextureFiltering(FT_MIN),
		textureUnit.getTextureFiltering(FT_MAG),
		textureUnit.getTextureFiltering(FT_MIP));/**/

	_setTexturelayerAnisotropy(texUnit, textureUnit.getTextureAnisotropy());
	_setTextureMipmapBias(texUnit, textureUnit.getTextureMipmapBias());
	_setTextureBlendMode(texUnit, textureUnit.getColorBlendMode());
	_setTextureBlendMode(texUnit, textureUnit.getAlphaBlendMode());

	const BWTextureUnitState::UVWAddressingMode uvw = textureUnit.getTextureAddressionMode();
	_setTextureAddressionMode(texUnit, uvw);

	if (uvw.u == BWTextureUnitState::TAM_BORDER ||
		uvw.v == BWTextureUnitState::TAM_BORDER ||
		uvw.w == BWTextureUnitState::TAM_BORDER )
	{
		_setTextureBorderColour(texUnit, textureUnit.getTextureBorderColour());
	}

	BWTextureUnitState::EffectMap::const_iterator itor;
	bool angCalcs = false;
	for (itor = textureUnit.getEffects().begin(); itor != textureUnit.getEffects().end(); itor++)
	{
		switch (itor->second.type)
		{
		case  BWTextureUnitState::ET_ENVIRONMENT_MAP:
			if (itor->second.subtype == BWTextureUnitState::ENV_CURVED)
			{
				_setTextureCoordCalculation(texUnit, TEXCALC_ENVIRONMENT_MAP);
			}
			else if (itor->second.subtype == BWTextureUnitState::ENV_PLANAR)
			{
				_setTextureCoordCalculation(texUnit, TEXCALC_ENVIRONMENT_MAP_PLANAR);
			}
			else if (itor->second.subtype == BWTextureUnitState::ENV_NORMAL)
			{
				_setTextureCoordCalculation(texUnit, TEXCALC_ENVIRONMENT_MAP_NORMAL);
			}
			else if (itor->second.subtype == BWTextureUnitState::ENV_REFLECTION)
			{
				_setTextureCoordCalculation(texUnit, TEXCALC_ENVIRONMENT_MAP_REFLECTION);
			}
			angCalcs = true;
			break;
		case BWTextureUnitState::ET_PROJECTIVE_TEXTURE:
			_setTextureCoordCalculation(texUnit, TEXCALC_PROJECTION_TEXTURE, itor->second.frustum);
			angCalcs = true;
		}
	}

	if (!angCalcs)
	{
		_setTextureCoordCalculation(texUnit, TEXCALC_NONE);
	}
	_setTextureMatrix(texUnit, textureUnit.getTextureTransform());
	return;
}

void BWRenderSystem::_render(const BWRenderOperation &ro)
{
	size_t val;
	if (ro.useIndexes)
	{
		val = ro.indexData->mIndexCount;
	}
	else
	{
		val = ro.vertexData->mVertexCount;
	}

	if (mCurrentPassIterationCount > 1)
	{
		val *= mCurrentPassIterationCount;
	}
	mCurrentPassIterationNum = 0;
	switch (ro.operationType)
	{
	case BWRenderOperation::OT_LINE_LIST:
		mFaceCount += val / 3;
		break;
	case BWRenderOperation::OT_LINE_STRIP:
	case BWRenderOperation::OT_TRIGANLE_FAN:
		mFaceCount += val - 2;
	default:
		break;
	}
	mVertexCount += ro.vertexData->mVertexCount;
	mBatchCount += mCurrentPassIterationCount;
	if (mClipPlanesDirty)
	{
		//assert(0);
		//setClipPlanesImp(mClipPlanes);
		mClipPlanesDirty = false;
	}
}

BWRenderWindow* BWRenderSystem::_initialise(bool autoCreateWindow, const std::string &windowTitle)
{
	mVertexProgramBound = false;
	mGeometryProgramBound = false;
	mFragmentProgramBound = false;
	return NULL;
}
void BWRenderSystem::_setWorldMatrices(const BWMatrix4* m, unsigned short count)
{
	_setWorldMatrix(BWMatrix4::IDENTITY);
}

void BWRenderSystem::unbindGpuProgram(GpuProgramType GPT)
{
	switch (GPT)
	{
	case GPT_VERTEX_PROGRAM:
		mVertexProgramBound = false;
	case GPT_FRAGMENT_PROGRAM:
		mFragmentProgramBound = false;
	case GPT_GEOMETRY_PROGRAM:
		mGeometryProgramBound = false;
	}
}
void BWRenderSystem::bindGPUProgramUsage(BWGpuProgramUsage*gpuPrgramUsage)
{
	assert(0);
}
bool BWRenderSystem::InitRendererResource()
{
	GRenderTarget = createRenderTarget("GRenderTarget");
	GRenderTarget->setWidth(1024);
	GRenderTarget->setHeight(768);

	BWTexturePtr texture = BWTextureManager::GetInstance()->Create("BaseColorMap", DEFAULT_RESOURCE_GROUP);
	texture->setHeight(GRenderTarget->getHeight());
	texture->setWidth(GRenderTarget->getWidth());
	GRenderTarget->addTextureBuffer(texture, 0);
	texture = BWTextureManager::GetInstance()->Create("NormalMap", DEFAULT_RESOURCE_GROUP);
	texture->setHeight(GRenderTarget->getHeight());
	texture->setWidth(GRenderTarget->getWidth());
	GRenderTarget->addTextureBuffer(texture, 0);
	texture = BWTextureManager::GetInstance()->Create("PositionMap", DEFAULT_RESOURCE_GROUP);
	texture->setHeight(GRenderTarget->getHeight());
	texture->setWidth(GRenderTarget->getWidth());
	GRenderTarget->addTextureBuffer(texture, 0);
	texture = BWTextureManager::GetInstance()->Create("AmbientOcclusion", DEFAULT_RESOURCE_GROUP);
	texture->setHeight(GRenderTarget->getHeight());
	texture->setWidth(GRenderTarget->getWidth());
	GRenderTarget->addTextureBuffer(texture, 0);


	GRenderTarget->createDepthBuffer(std::string("DepthBuffer"));
	GRenderTarget->createPixelBuffer(std::string("FinalRenderResult"));

	AmbientOcclusionMaterial = BWMaterialManager::GetInstance()->GetResource("AmbientOcclusion", "General");

	mPointLightMesh = BWMeshManager::GetInstance()->load("sphere.mesh", "General");
	mCubeMesh = BWMeshManager::GetInstance()->load("cube.mesh", "General");


	//mDirectionLightM = BWMaterialManager::GetInstance()->GetResource("DirectLightDefferRendering", "General");
	mDirectionLightM = BWMaterialManager::GetInstance()->GetResource("Ogre/BaseDirectLightingMaterial", "General");
	if (mDirectionLightM.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mDirectionLightM material");
		return false;
	}
	mPointLightM = BWMaterialManager::GetInstance()->GetResource("PointLightDefferLightting", "General");
	if (mPointLightM.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mDirectionLightM material");
		return false;
	}


	BaseColorTexture = GRenderTarget->getTextureBuffer("BaseColorMap");
	NormalTexture = GRenderTarget->getTextureBuffer("NormalMap");
	PositionTexture = GRenderTarget->getTextureBuffer("PositionMap");

	//阴影相关
	/*ShadowMapTarget = createRenderTarget("ShadowRenderTarget");
	ShadowMapTarget->setWidth(1024);
	ShadowMapTarget->setHeight(768);

	DirectionLightShadowMapM = BWMaterialManager::GetInstance()->GetResource("DirectLightShadowMap", "General");
	if (DirectionLightShadowMapM.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the DirectLightShadowMap material");
		return false;
	}
	PointLightShadowMapM = BWMaterialManager::GetInstance()->GetResource("PointLightShadowMap", "General");

	if (PointLightShadowMapM.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the PointLightShadowMap material");
		return false;
	}*/


	mSkyBoxM = BWMaterialManager::GetInstance()->GetResource("SkyBox", "General");
	if (mSkyBoxM.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mSkyBoxM material");
		return false;
	}
	return true;
}
///////////////////New Interface
void BWRenderSystem::SetRenderTarget(RSRenderTarget& RenderTarget)
{
	  
}

void BWRenderSystem::SetGrphicsPipelineState(RSGraphicPipelineState& InPipelineState)
{
	
}

void BWRenderSystem::SetShaderTexture(BWGpuProgramPtr GPUProgram, BWTexturePtr Texture, StaticSamplerStateHIPtr Sampler)
{
	
}

//////////////////New Interface End;
void BWRenderSystem::RenderLightsShadowMaps()
{
	DirectionalLightMap* DirectLightMap = GlobalData->GetActiveLightMap<DirectionalLightMap>();
	ObjectsQueneWithInLightMap & ObjectsQueueInLight = *GlobalData->GetObjectesQueueInLight();
	for (auto Light : *DirectLightMap)
	{
		BWRenderQueue &ObjectsQueueToBeRendered = *ObjectsQueueInLight[Light.first];
		for (int i = 0; i < Light.second->GetCastShadowMapNum(); i++)
		{
			ShadowMapProjectInfo& ProjectInfo = Light.second->GetShadowMapProjectedInfor(i);
			SetProjectedShadowInfoForRenderShadow(ProjectInfo);

			for ( auto GroupList : ObjectsQueueToBeRendered.getRenderQueueGroupList())
			{
				BWRenderQueueGroup* renderQueueGroup = GroupList.second;
				for (auto PriorityGroup : renderQueueGroup->getRenderPriorityGroupList())
				{
					BWQueueRenderableCollection::PassRenderableList &RenderableListBasedPass = PriorityGroup.second->getSolidsBasic().GetPassRenderableList();
					for (auto RenderableList : RenderableListBasedPass)
					{
						for (auto Renderable : *RenderableList.second)
						{
							RenderRenderableShadow(const_cast<BWRenderable*>(Renderable));
						}
					}
				}
			}
			RemoveProjectedShadowInfoFromRenderShadow(ProjectInfo);
		}
	}
	FinishLightsShadowMaps();
}
void BWRenderSystem::RenderLights()
{
	//RenderLightsShadowMaps();
	BeginDeferLight();
	DirectLightPass();
	PointLightPass();
}

void BWRenderSystem::RenderInDirectLights()
{

}

void BWRenderSystem::BeginDeferLight()
{
	
}
void BWRenderSystem::PointLightPass()
{

}
void BWRenderSystem::DirectLightPass()
{

}