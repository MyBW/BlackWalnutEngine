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
#include "..\RenderSystem_GL\GLHardwareDepthBuffer.h"
#include "BWStringConverter.h"
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


	CopyTextureToScreen(BWRenderSystem::FinalRenderResult, 0, 0);
	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BWRenderSystem::FinalRenderResult;
	SetRenderTarget(nullptr, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR|FBT_DEPTH);
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


void BWRenderSystem::SetupGBufferRenderTarget(BWGpuProgramUsagePtr GPUUsage)
{
	RSRenderTargets RenderTargets;
	RSRenderTarget RenderTarget;
	RenderTargets.DepthAndStencil = GDepthBuffer;
	RenderTargets.GPUProgramUsage = GPUUsage;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BaseColorTexture;
	RenderTargets.RenderTargets.push_back(RenderTarget);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = NormalTexture;
	RenderTargets.RenderTargets.push_back(RenderTarget);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = PositionTexture;
	RenderTargets.RenderTargets.push_back(RenderTarget);
	SetRenderTargets(RenderTargets);
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

	FinalRenderResult = BWTextureManager::GetInstance()->Create("FinalRenderReslutColor", DEFAULT_RESOURCE_GROUP);
	FinalRenderResult->setWidth(BaseColorTexture->getWidth());
	FinalRenderResult->setHeight(BaseColorTexture->getHeight());
	FinalRenderResult->setTextureType(TEX_TYPE_2D);
	FinalRenderResult->setFormat(PF_BYTE_BGRA);
	FinalRenderResult->setNumMipmaps(0);
	FinalRenderResult->SetIndex(0);
	FinalRenderResult->createInternalResources();
	
	GDepthBuffer = GRenderTarget->getDepthRenderBuffer(std::string("DepthBuffer"));

	DirectLightProgramUsage = mDirectionLightM->getTechnique(0)->GetPass(0)->getGPUProgramUsage();

	BWMaterialPtr ImageBaseLightingMaterial = BWMaterialManager::GetInstance()->GetResource("ImageBaseLighting", "General");
	ImageBaseLightingMaterial->Load();
	ImageBaseLightingUsage = ImageBaseLightingMaterial->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	ImageBaseLighting = ImageBaseLightingUsage->GetHighLevelGpuProgram();
	ImageBaseLighting->Load();




	
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
	mSkyBoxM->Load();
	mSkyBoxGpuPrgramUsage = mSkyBoxM->getTechnique(0)->GetPass(0)->getGPUProgramUsage();

	BWMeshPrt mCubeMesh = GetCubeMesh();
	BWSubMesh *mCubeSubMesh = mCubeMesh->getSubMesh(0);
	mCubeSubMesh->_getRenderOperation(CubeMeshRenderOperation);

	BWQuaternion Quaterniton;
	BWMatrix4 ViewMatrixs[6];
	//以下的方向都是相对世界坐标系来说的 左手坐标系 这里要注意旋转角度的正负对方向的影响
	Quaterniton.fromAngleAxis(Radian(-PI / 2), BWVector3D(0.0, 1.0, 0.0));
	ViewMatrixs[0] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // +X
	Quaterniton.fromAngleAxis(Radian(PI / 2), BWVector3D(0.0, 1.0, 0.0));
	ViewMatrixs[1] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); //-X
	Quaterniton.fromAngleAxis(Radian(-PI / 2.0), BWVector3D(1.0, 0.0, 0.0));
	ViewMatrixs[2] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // +Y
	Quaterniton.fromAngleAxis(Radian(PI / 2.0), BWVector3D(1.0, 0.0, 0.0));
	ViewMatrixs[3] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // -Y
	Quaterniton.fromAngleAxis(Radian(0), BWVector3D(0.0, 1.0, 0.0));
	ViewMatrixs[4] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // +Z
	Quaterniton.fromAngleAxis(Radian(PI), BWVector3D(0.0, 1.0, 0.0));
	ViewMatrixs[5] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); //-Z



	mProcessEvnMap = BWMaterialManager::GetInstance()->GetResource("ProcessEnvMap", "General");
	if (mProcessEvnMap.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mProcessEvnMap material");
		return false;
	}
	mProcessEvnMap->Load();
	mProcessEvnMapTexture = mProcessEvnMap->getTechnique(0)->GetPass(0)->getTextureUnitState(0)->_getTexturePtr();
	mProcessEvnMapGpuPrgramUsage = mProcessEvnMap->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	mProcessEvnMapProgram = mProcessEvnMapGpuPrgramUsage->GetHighLevelGpuProgram();
	mProcessEvnMapProgram->Load();



	//从Equirectangular HRD 产生 CubeMap环境贴图
	mConverEquirectangularToCubeMap = BWMaterialManager::GetInstance()->GetResource("ConvertEquirectangularToCubeMap", "General");
	if (mConverEquirectangularToCubeMap.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the ConverEquirectangularToCubeMap material");
		return false;
	}
	//目前使用这样的方式创建贴图  希望能换一种方式 例如 直接New一个贴图BWTextureUnitState
	BWTextureUnitState* BeConveredMap = mConverEquirectangularToCubeMap->getTechnique(0)->GetPass(0)->getTextureUnitState(0);
	//BeConveredMap->setTextureName("GCanyon_C_YumaPoint_3k.hdr");
	BeConveredMap->setTextureName("small_apartment.hdr");
	mConverEquirectangularToCubeMap->Load();
	BeConveredMap->SetIndex(0);
	BeConveredMap->_getTexturePtr()->setNumMipmaps(0);
	BWGpuProgramUsagePtr mConverEquirectangularToCubeMapUsage = mConverEquirectangularToCubeMap->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	BWHighLevelGpuProgramPtr mConverEquirectangularToCubeMapGPU = mConverEquirectangularToCubeMap->getTechnique(0)->GetPass(0)->getGPUProgramUsage()->GetHighLevelGpuProgram();
	mConverEquirectangularToCubeMapGPU->Load();

	HDRCubeMap = BWTextureManager::GetInstance()->Create("HDR_Cube_Map", "General");
	HDRCubeMap->setTextureType(TEX_TYPE_CUBE_MAP);
	HDRCubeMap->setFormat(PF_FLOAT32_RGB);
	HDRCubeMap->setWidth(mProcessEvnMapTexture->getWidth());
	HDRCubeMap->setHeight(mProcessEvnMapTexture->getHeight());
	HDRCubeMap->setNumMipmaps(0);
	HDRCubeMap->createInternalResources();
	BWHardwareDepthBufferPtr TmpDepthBuffer =
		new GLHardwareDepthBuffer("TempRenderTarget", HDRCubeMap->getWidth(), HDRCubeMap->getHeight(), 0, nullptr, BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
	TmpDepthBuffer->SetIsWithStencil(false);


	RSGraphicPipelineState PipeLineState;
	RSRenderTarget RenderTarget;
	PipeLineState.DepthAndStencilState = TStaticDepthAndStencilState<true, true, CMPF_LESS_EQUAL>::GetStateHI();
	PipeLineState.RasterizerState = TStaticRasterizerState<PM_SOLID, CULL_CLOCKWISE>::GetStateHI();
	//PipeLineState.GPUProgram = mConverEquirectangularToCubeMapGPU;
	PipeLineState.GPUProgramUsage = mConverEquirectangularToCubeMapUsage;
	RenderTarget.RenderTargetTexture = HDRCubeMap;

	SetViewport(0, 0, HDRCubeMap->getWidth(), HDRCubeMap->getHeight());
	SetGraphicsPipelineState(PipeLineState);
	SetShaderTexture(mConverEquirectangularToCubeMapGPU, BeConveredMap->_getTexturePtr(), TStaticSamplerState<FO_LINEAR>::GetStateHI());
	for (int i = 0; i < 6; i++)
	{
		RenderTarget.Index = i;
		RenderTarget.MipmapLevel = 0;
		SetRenderTarget(mConverEquirectangularToCubeMapUsage, RenderTarget ,TmpDepthBuffer);
		mConverEquirectangularToCubeMapUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[i]);
		ClearRenderTarget(FBT_DEPTH);
		BWHighLevelGpuProgramPtr tmp;
		RenderOperation(CubeMeshRenderOperation, tmp);
	}
	// 从Equirectangular HRD 产生 CubeMap环境贴图 End


	//Use SH To Convolution The EnvMap
	BWHighLevelGpuProgramPtr SHConvolution;
	BWHighLevelGpuProgramPtr AccumulateDiffuse;
	BWHighLevelGpuProgramPtr AccumulateReadCubeFace;
	BWGpuProgramUsagePtr SHConvolutonUsage;
	BWGpuProgramUsagePtr AccumulateUsage;
	BWGpuProgramUsagePtr AccumulateReadCubeFaceUsage;

	BWMaterialPtr SHMaterial = BWMaterialManager::GetInstance()->GetResource("SHConvolution", "General");
	SHMaterial->Load();
	SHConvolutonUsage = SHMaterial->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	SHConvolution = SHConvolutonUsage->GetHighLevelGpuProgram();
	SHConvolution->Load();

	BWMaterialPtr AccumulateDiffuseMaterial = BWMaterialManager::GetInstance()->GetResource("AccumulateDiffuse", "General");
	AccumulateDiffuseMaterial->Load();
	AccumulateUsage = AccumulateDiffuseMaterial->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	AccumulateDiffuse = AccumulateUsage->GetHighLevelGpuProgram();
	AccumulateDiffuse->Load();


	BWMaterialPtr AccumulateReadCubeMapMaterial = BWMaterialManager::GetInstance()->GetResource("AccumulateReadCubeMap", "General");
	AccumulateReadCubeMapMaterial->Load();
	AccumulateReadCubeFaceUsage = AccumulateReadCubeMapMaterial->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	AccumulateReadCubeFace = AccumulateReadCubeFaceUsage->GetHighLevelGpuProgram();
	AccumulateReadCubeFace->Load();


	float Sampler01[4] = { 0.0f ,0.0f ,0.0f, 0.0f };
	float Sampler23[4] = { 0.0f ,0.0f ,0.0f, 0.0f };
	BWTexturePtr GatherSHReslut;
	float Mask[TSHVector<3>::MaxBasis];
	const int CubeMapWidth = mProcessEvnMapTexture->getWidth();
	const int CubeMapHigh = mProcessEvnMapTexture->getHeight();
	const int MipLevelNum = HelperFunction::GetMaxMipmaps(CubeMapWidth, CubeMapHigh, 1);
	int CurrentMipLevel = 0;
	BWHardwareDepthBufferPtr SHDepthBuffer =
		new GLHardwareDepthBuffer("TempRenderTarget", CubeMapWidth, CubeMapHigh, 0, nullptr, BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
	SHDepthBuffer->SetIsWithStencil(false);
	for (int TextureIndx = 0; TextureIndx < 2; TextureIndx++)
	{
		std::string TextureName = std::string("SH_Cube_Map") + StringConverter::ToString(TextureIndx);
		AccumulationCubeMaps[TextureIndx] = BWTextureManager::GetInstance()->Create(TextureName, "General");
		AccumulationCubeMaps[TextureIndx]->setTextureType(TEX_TYPE_CUBE_MAP);
		AccumulationCubeMaps[TextureIndx]->setFormat(PF_FLOAT32_RGBA);
		AccumulationCubeMaps[TextureIndx]->setWidth(CubeMapWidth);
		AccumulationCubeMaps[TextureIndx]->setHeight(CubeMapHigh);
		AccumulationCubeMaps[TextureIndx]->setNumMipmaps(MipLevelNum);
		AccumulationCubeMaps[TextureIndx]->SetIndex(0);
		AccumulationCubeMaps[TextureIndx]->createInternalResources();
	}
	GatherSHReslut = BWTextureManager::GetInstance()->Create("GatherSHReslut", "General");
	GatherSHReslut->setTextureType(TEX_TYPE_2D);
	GatherSHReslut->setFormat(PF_FLOAT32_RGBA);
	GatherSHReslut->setHeight(1);
	GatherSHReslut->setWidth(SHVector.MaxBasis);
	GatherSHReslut->setNumMipmaps(0);
	GatherSHReslut->SetIndex(0);
	GatherSHReslut->createInternalResources();

	for (int i = 0; i < SHVector.MaxBasis; i++)
	{
		CurrentMipLevel = 0;
		for (int k = 0; k < SHVector.MaxBasis; k++)
		{
			Mask[k] = i == k ? 1.0f : 0.0f;
		}
		//PipeLineState.GPUProgram = SHConvolution;
		PipeLineState.GPUProgramUsage = SHConvolutonUsage;
		RenderTarget.RenderTargetTexture = AccumulationCubeMaps[CurrentMipLevel % 2];

		SetGraphicsPipelineState(PipeLineState);
		SetShaderTexture(SHConvolution, HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());
		SetViewport(0, 0, CubeMapWidth, CubeMapHigh);

		for (int CubeFace = 0; CubeFace < 6; CubeFace++)
		{
			RenderTarget.Index = CubeFace;
			RenderTarget.MipmapLevel = CurrentMipLevel;
			SetRenderTarget( SHConvolutonUsage,RenderTarget, TmpDepthBuffer);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("Mask0", Mask, 4, 1);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("Mask1", &Mask[4], 4, 1);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("Mask2", &Mask[8], 1, 1);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[CubeFace]);
			//SHConvolution->SetGPUProgramParameters(SHConvolutonUsage->GetGpuProgramParameter());
			ClearRenderTarget(FBT_DEPTH);
			//RenderOperation(CubeMeshRenderOperation, dynamic_cast<GLSLGpuProgram*>(SHConvolution.Get()));
			BWHighLevelGpuProgramPtr tmp;
			//RenderOperation(CubeMeshRenderOperation, SHConvolution);
			RenderOperation(CubeMeshRenderOperation, tmp);
		}
		//PipeLineState.GPUProgram = AccumulateDiffuse;
		PipeLineState.GPUProgramUsage = AccumulateUsage;
		for (CurrentMipLevel = 1; CurrentMipLevel < MipLevelNum; CurrentMipLevel++)
		{
			float MipScale = std::pow<float>(0.5, CurrentMipLevel);
			int PreMipLevel = CurrentMipLevel - 1;

			int MipSize = 1 << (MipLevelNum - PreMipLevel - 1);
			float HalfSourceTexelSize = .5f / MipSize;
			Sampler01[0] = -HalfSourceTexelSize;
			Sampler01[1] = -HalfSourceTexelSize;
			Sampler01[2] = HalfSourceTexelSize;
			Sampler01[3] = -HalfSourceTexelSize;
			Sampler23[0] = -HalfSourceTexelSize;
			Sampler23[1] = HalfSourceTexelSize;
			Sampler23[2] = HalfSourceTexelSize;
			Sampler23[3] = HalfSourceTexelSize;

			RenderTarget.MipmapLevel = CurrentMipLevel;
			RenderTarget.RenderTargetTexture = AccumulationCubeMaps[CurrentMipLevel % 2];

			SetViewport(0, 0, CubeMapWidth * MipScale, CubeMapHigh * MipScale);
			SetShaderTexture(AccumulateDiffuse, AccumulationCubeMaps[1 - CurrentMipLevel % 2], TStaticSamplerState<FO_LINEAR>::GetStateHI());
			SetGraphicsPipelineState(PipeLineState);
			for (int CubeFace = 0; CubeFace < 6; CubeFace++)
			{
				RenderTarget.Index = CubeFace;
				AccumulateUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[CubeFace]);
				AccumulateUsage->GetGpuProgramParameter()->SetNamedConstant("Sample01", Sampler01, 4, 1);
				AccumulateUsage->GetGpuProgramParameter()->SetNamedConstant("Sample23", Sampler23, 4, 1);
				AccumulateUsage->GetGpuProgramParameter()->SetNamedConstant("CubeFace", &CubeFace, 1, 1);
				AccumulateUsage->GetGpuProgramParameter()->SetNamedConstant("MipMapLevel", &PreMipLevel, 1, 1);
				//AccumulateDiffuse->SetGPUProgramParameters(AccumulateUsage->GetGpuProgramParameter());
				SetRenderTarget(AccumulateUsage ,RenderTarget, TmpDepthBuffer);
				ClearRenderTarget(FBT_DEPTH);
				//RenderOperation(CubeMeshRenderOperation, dynamic_cast<GLSLGpuProgram*>(AccumulateDiffuse.Get()));
				//RenderOperation(CubeMeshRenderOperation, AccumulateDiffuse);
				BWHighLevelGpuProgramPtr tmp;
				RenderOperation(CubeMeshRenderOperation, tmp);
			}
		}
		//PipeLineState.GPUProgram = AccumulateReadCubeFace;
		PipeLineState.GPUProgramUsage = AccumulateReadCubeFaceUsage;
		SetViewport(0, 0, SHVector.MaxBasis, 1);
		SetScissor(true, i, 0, i + 1, 1);

		SetGraphicsPipelineState(PipeLineState);
		RenderTarget.Index = 0;
		RenderTarget.MipmapLevel = 0;
		RenderTarget.RenderTargetTexture = GatherSHReslut;
		SetRenderTarget(AccumulateReadCubeFaceUsage, RenderTarget, TmpDepthBuffer);
		SetShaderTexture(AccumulateReadCubeFace, AccumulationCubeMaps[(MipLevelNum - 1) % 2], TStaticSamplerState<FO_LINEAR>::GetStateHI());
		ClearRenderTarget(FBT_DEPTH);
		CurrentMipLevel--;
		AccumulateReadCubeFaceUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[0]);
		AccumulateReadCubeFaceUsage->GetGpuProgramParameter()->SetNamedConstant("MipMapLevel", &CurrentMipLevel, 1, 1);
		//AccumulateReadCubeFace->SetGPUProgramParameters(AccumulateReadCubeFaceUsage->GetGpuProgramParameter());
		//RenderOperation(CubeMeshRenderOperation, dynamic_cast<GLSLGpuProgram*>(AccumulateReadCubeFace.Get()));
		BWHighLevelGpuProgramPtr tmp;
		RenderOperation(CubeMeshRenderOperation, tmp);
		SetScissor(false);
	}

	// 目前发现一个问题 PixelData数据中的alpha通道一直是1.0  不论如何调整shader的输出值 一直保持不变
	/*GLenum DesFormat = GLPixelUtil::getGLOriginFormat(PF_FLOAT32_RGBA);
	GLenum DesDataType = GLPixelUtil::getGLOriginDataType(PF_FLOAT32_RGBA);
	float PixelData[36];
	{
	GLTexture* Texture = dynamic_cast<GLTexture*>(GatherSHReslut.Get());
	glBindTexture(GL_TEXTURE_2D, Texture->GetHIID());
	glGetTexImage(GL_TEXTURE_2D, 0, DesFormat, DesDataType, PixelData);
	}
	*/

	BWPixelBox PixelBox(SHVector.MaxBasis, 1, 0, PixelFormat::PF_FLOAT32_RGBA, SHVector.V);
	ReadSurfaceData(GatherSHReslut, 0, 0, PixelBox);




	IBL_Diffuse_Cube_Map = BWTextureManager::GetInstance()->Create("IBL_Diffuse_Cube_Map", "General");
	IBL_Diffuse_Cube_Map->setTextureType(TEX_TYPE_CUBE_MAP);
	IBL_Diffuse_Cube_Map->setFormat(PF_FLOAT32_RGB);
	IBL_Diffuse_Cube_Map->setWidth(mProcessEvnMapTexture->getWidth());
	IBL_Diffuse_Cube_Map->setHeight(mProcessEvnMapTexture->getHeight());
	IBL_Diffuse_Cube_Map->setNumMipmaps(0);
	IBL_Diffuse_Cube_Map->createInternalResources();
	HDRCubeMap->SetIndex(0);
	BWHardwareDepthBufferPtr TmpDepthBuffer1 =
		new GLHardwareDepthBuffer("TempRenderTarget1", IBL_Diffuse_Cube_Map->getWidth(), IBL_Diffuse_Cube_Map->getHeight(), 0, nullptr, BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
	TmpDepthBuffer1->SetIsWithStencil(false);
	SetShaderTexture(mProcessEvnMapProgram, HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	SetViewport(0, 0, IBL_Diffuse_Cube_Map->getWidth(), IBL_Diffuse_Cube_Map->getHeight());
	//PipeLineState.GPUProgram = mProcessEvnMapProgram;
	PipeLineState.GPUProgramUsage = mProcessEvnMapGpuPrgramUsage;
	SetGraphicsPipelineState(PipeLineState);

	RenderTarget.RenderTargetTexture = IBL_Diffuse_Cube_Map;
	for (int i = 0; i < 6; i++)
	{
		RenderTarget.Index = i;
		RenderTarget.MipmapLevel = 0;
		SetRenderTarget(mProcessEvnMapGpuPrgramUsage ,RenderTarget,TmpDepthBuffer1);
		mProcessEvnMapGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[i]);
		ClearRenderTarget(FBT_DEPTH);
		BWHighLevelGpuProgramPtr tmp;
		RenderOperation(CubeMeshRenderOperation, tmp);
	}

	//环境贴图对高光处理
	mPreprocessEvnMapForSpecular = BWMaterialManager::GetInstance()->GetResource("ProcessEnvMapForSpecular", "General");
	if (mPreprocessEvnMapForSpecular.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mProcessEvnMap material");
		return false;
	}
	mPreprocessEvnMapForSpecular->Load();
	mProcessEvnMapForSpecularGpuPrgramUsage = mPreprocessEvnMapForSpecular->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	mProcessEvnMapForSpecularProgram = mProcessEvnMapForSpecularGpuPrgramUsage->GetHighLevelGpuProgram();
	mProcessEvnMapForSpecularProgram->Load();

	const int RoughnessLevel = 7;
	IBL_Specular_Cube_Map = BWTextureManager::GetInstance()->Create("IBL_Specular_Cube_Map", "General");
	IBL_Specular_Cube_Map->setTextureType(TEX_TYPE_CUBE_MAP);
	IBL_Specular_Cube_Map->setFormat(PF_FLOAT32_RGB);
	IBL_Specular_Cube_Map->setWidth(mProcessEvnMapTexture->getWidth());
	IBL_Specular_Cube_Map->setHeight(mProcessEvnMapTexture->getHeight());
	IBL_Specular_Cube_Map->setNumMipmaps(RoughnessLevel);
	IBL_Specular_Cube_Map->createInternalResources();


	int Width = mProcessEvnMapTexture->getWidth();
	int Height = mProcessEvnMapTexture->getHeight();
	//PipeLineState.GPUProgram = mProcessEvnMapForSpecularProgram;
	PipeLineState.GPUProgramUsage = mProcessEvnMapForSpecularGpuPrgramUsage;
	SetGraphicsPipelineState(PipeLineState);

	HDRCubeMap->SetIndex(0);
	SetShaderTexture(mProcessEvnMapForSpecularProgram, HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	RenderTarget.RenderTargetTexture = IBL_Specular_Cube_Map;
	//RenderTarget.DepthAndStencil = TmpDepthBuffer1;
	for (int mip = 0; mip < RoughnessLevel; mip++)
	{
		int MipWidth = Width * std::pow(0.5, mip);
		int MipHeight = Height  * std::pow(0.5, mip);
		SetViewport(0, 0, MipWidth, MipHeight);
		RenderTarget.MipmapLevel = mip;
		float roughness = mip / float(RoughnessLevel - 1);
		for (int i = 0; i < 6; i++)
		{
			RenderTarget.Index = i;
			SetRenderTarget(mProcessEvnMapForSpecularGpuPrgramUsage ,RenderTarget, TmpDepthBuffer1);
			mProcessEvnMapForSpecularGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[i]);
			mProcessEvnMapForSpecularGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("roughness", &roughness, 1, 1);
			//mProcessEvnMapForSpecularProgram->SetGPUProgramParameters(mProcessEvnMapForSpecularGpuPrgramUsage->GetGpuProgramParameter());
			ClearRenderTarget(FBT_DEPTH);
			BWHighLevelGpuProgramPtr tmp;
			//RenderOperation(CubeMeshRenderOperation, dynamic_cast<GLSLGpuProgram*>(mProcessEvnMapForSpecularProgram.Get()));
			//RenderOperation(CubeMeshRenderOperation, mProcessEvnMapForSpecularProgram);
			RenderOperation(CubeMeshRenderOperation, tmp);
		}
	}



	//计算LUT
	mPreprocessEvnMapLUT = BWMaterialManager::GetInstance()->GetResource("PreprocessEvnMapLUT", "General");
	if (mPreprocessEvnMapLUT.IsNull())
	{
		Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mProcessEvnMap material");
		return false;
	}
	mPreprocessEvnMapLUT->Load();
	mProcessEvnMapLUTGpuPrgramUsage = mPreprocessEvnMapLUT->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
	mProcessEvnMapLUTProgram = mProcessEvnMapLUTGpuPrgramUsage->GetHighLevelGpuProgram();
	mProcessEvnMapLUTProgram->Load();

	IBL_LUT = BWTextureManager::GetInstance()->Create("IBL_LUT", "General");
	IBL_LUT->setTextureType(TEX_TYPE_2D);
	IBL_LUT->setWidth(512);
	IBL_LUT->setHeight(512);
	IBL_LUT->setNumMipmaps(0);
	IBL_LUT->createInternalResources();
	SetViewport(0, 0, 512, 512);

	//PipeLineState.GPUProgram = mProcessEvnMapLUTProgram;
	PipeLineState.GPUProgramUsage = mProcessEvnMapLUTGpuPrgramUsage;
	SetGraphicsPipelineState(PipeLineState);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = IBL_LUT;
	SetRenderTarget(mProcessEvnMapLUTGpuPrgramUsage, RenderTarget, TmpDepthBuffer1);
	ClearRenderTarget(FBT_DEPTH);
	//RenderOperation(CubeMeshRenderOperation, dynamic_cast<GLSLGpuProgram*>(mProcessEvnMapLUTProgram.Get()));
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);

	return true;
}

void BWRenderSystem::SetViewport(int ViewportX, int ViewportY, int ViewportWidth, int ViewportHight)
{

}


void BWRenderSystem::SetScissor(bool IsEnable, int ScissorX, int ScissorY, int ScissorWidth, int ScissorHigh)
{

}

///////////////////New Interface

void BWRenderSystem::SetGraphicsPipelineState(RSGraphicPipelineState& InPipelineState)
{
	if (CachedPipelineState.RasterizerState != InPipelineState.RasterizerState)
	{
		CachedPipelineState.RasterizerState = InPipelineState.RasterizerState;
		CachedPipelineState.RasterizerState->IsDirty = true;
	}
	if (CachedPipelineState.DepthAndStencilState != InPipelineState.DepthAndStencilState)
	{
		CachedPipelineState.DepthAndStencilState = InPipelineState.DepthAndStencilState;
		CachedPipelineState.DepthAndStencilState->IsDirty = true;
	}
	if (CachedPipelineState.BlendState != InPipelineState.BlendState)
	{
		CachedPipelineState.BlendState = InPipelineState.BlendState;
		CachedPipelineState.BlendState->IsDirty = true;
	}
	CachedPipelineState.GPUProgramUsage = InPipelineState.GPUProgramUsage;
}

void BWRenderSystem::SetShaderTexture(BWHighLevelGpuProgramPtr GPUProgram, BWTexturePtr Texture, SamplerStateHIRef Sampler)
{
	RSShaderTexture ShaderTexure;
	ShaderTexure.GPUProgramUsage = GPUProgram; 
	ShaderTexure.Sampler = Sampler;
	if (ShaderTexure.Sampler != Sampler)
	{
		ShaderTexure.Sampler = Sampler;
		ShaderTexure.Sampler->IsDirty = true;
	}
	ShaderTexure.Texture = Texture;
	ShaderTextures.push_back(ShaderTexure);
}

void BWRenderSystem::ClearRenderTarget(unsigned int buffers, const ColourValue &color /*= ColourValue::Black*/, float depth /*= 1.0*/, unsigned short stencil /*= 0*/)
{
	
}


void BWRenderSystem::ClearResourceForRender()
{
	ShaderTextures.clear();
	CachedPipelineState.GPUProgramUsage.SetNull();
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
	return;
    // Image Base Lighting 
	RSGraphicPipelineState PipelineState;
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<>::GetStateHI();
	PipelineState.GPUProgramUsage = ImageBaseLightingUsage;
	PipelineState.RasterizerState = TStaticRasterizerState<PM_SOLID, CULL_CLOCKWISE>::GetStateHI();
	PipelineState.BlendState = TStaticBlendStateHI<true, SBO_ADD, SBF_ONE, SBF_ONE>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);

	BaseColorTexture->SetIndex(0);
	NormalTexture->SetIndex(1);
	PositionTexture->SetIndex(2);
	IBL_Specular_Cube_Map->SetIndex(3);
	IBL_LUT->SetIndex(4);
	SetShaderTexture(ImageBaseLighting, BaseColorTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, NormalTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, PositionTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, IBL_Specular_Cube_Map, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, IBL_LUT, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = FinalRenderResult;
	SetRenderTarget(ImageBaseLightingUsage, RenderTarget,GDepthBuffer);

	ImageBaseLightingUsage->GetGpuProgramParameter()->SetNamedConstant("SHCoefficient", SHVector.V, 36, 1);
	ImageBaseLighting->SetGPUProgramParameters(ImageBaseLightingUsage->GetGpuProgramParameter());
	ClearRenderTarget(FBT_DEPTH);
	RenderOperation(CubeMeshRenderOperation, ImageBaseLighting);

	PipelineState.BlendState = TStaticBlendStateHI<false, SBO_ADD, SBF_ONE, SBF_ONE>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);
}

void BWRenderSystem::BeginDeferLight()
{
	
}
void BWRenderSystem::PointLightPass()
{

}

void BWRenderSystem::RenderSkyBox()
{
	RSGraphicPipelineState PipeLine;
	RSRenderTarget RenderTarget;
	BWHighLevelGpuProgramPtr tmp;

	PipeLine.GPUProgramUsage = mSkyBoxGpuPrgramUsage;
	PipeLine.RasterizerState = TStaticRasterizerState<PM_SOLID, CULL_CLOCKWISE>::GetStateHI();
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BWRenderSystem::FinalRenderResult;
	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->SetGPUAutoParameter(mSkyBoxGpuPrgramUsage->GetGpuProgramParameter());

	SetGraphicsPipelineState(PipeLine);
	SetShaderTexture(mSkyBoxGpuPrgramUsage->GetHighLevelGpuProgram(), HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetRenderTarget(mSkyBoxGpuPrgramUsage, RenderTarget, GDepthBuffer);
	RenderOperation(CubeMeshRenderOperation, tmp);

	PipeLine.RasterizerState = TStaticRasterizerState<PM_SOLID, CULL_ANTICLOCKWISE>::GetStateHI();
	SetGraphicsPipelineState(PipeLine);

}

void BWRenderSystem::DirectLightPass()
{
	RSGraphicPipelineState Pipeline;
	BWHighLevelGpuProgramPtr DirectLighting;
	Pipeline.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	Pipeline.BlendState = TStaticBlendStateHI<true, SBO_ADD, SBF_ONE, SBF_ONE>::GetStateHI();
	Pipeline.GPUProgramUsage = DirectLightProgramUsage;
	SetGraphicsPipelineState(Pipeline);
	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->SetGPUAutoParameter(
		DirectLightProgramUsage->GetGpuProgramParameter()
		);
	DirectLighting = DirectLightProgramUsage->GetHighLevelGpuProgram();

	BaseColorTexture->SetIndex(0);
	NormalTexture->SetIndex(1);
	PositionTexture->SetIndex(2);
	IBL_Diffuse_Cube_Map->SetIndex(3);
	IBL_Specular_Cube_Map->SetIndex(4);
	IBL_LUT->SetIndex(5);
	SetShaderTexture(DirectLighting, BaseColorTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, NormalTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, PositionTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, IBL_Diffuse_Cube_Map, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, IBL_Specular_Cube_Map, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, IBL_LUT, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BWRenderSystem::FinalRenderResult;
	SetRenderTarget(DirectLightProgramUsage, RenderTarget, GDepthBuffer);
	//GLint ShadowIndexLocation = glGetUniformLocation(DirectLightGLSLProgram->getID(), "ShadowIndex");
	//GLint FrustumShadowEndWorldSpace0Location = glGetUniformLocation(DirectLightGLSLProgram->getID(), "FrustumShadowEndWorldSpace0");
	//GLint FrustumShadowEndWorldSpace1Location = glGetUniformLocation(DirectLightGLSLProgram->getID(), "FrustumShadowEndWorldSpace1");
	//GLint FrustumShadowEndWorldSpace2Location = glGetUniformLocation(DirectLightGLSLProgram->getID(), "FrustumShadowEndWorldSpace2");
	//GLint FrustumShadowEndWorldSpace3Location = glGetUniformLocation(DirectLightGLSLProgram->getID(), "FrustumShadowEndWorldSpace3");

	DirectionalLightMap& Lights = BWRoot::GetInstance()->getSceneManager()->GetActiveDirectionalLights();
	DirectionalLightMap::iterator Light = Lights.begin();
	while (Light != Lights.end())
	{
		BWDirectionalLight* DirLight = Light->second;
		BWVector3D LightDir = DirLight->GetDirection();
		ColourValue LightColor = DirLight->GetLightColor();
		float ColorValue[3]; ColorValue[0] = LightColor.r; ColorValue[1] = LightColor.g; ColorValue[2] = LightColor.b;
		DirectLightProgramUsage->GetGpuProgramParameter()->SetNamedConstant("LightColor", ColorValue, 3, 1);
		float Dir[3]; Dir[0] = -LightDir.x; Dir[1] = -LightDir.y; Dir[2] = -LightDir.z;
		DirectLightProgramUsage->GetGpuProgramParameter()->SetNamedConstant("LightDirection", Dir, 3, 1);
		//CHECK_GL_ERROR(glUniform4f(FrustumShadowEndWorldSpace0Location,
		//	DirLight->FrustumEndWorldSpace[0].x,
		//	DirLight->FrustumEndWorldSpace[0].y,
		//	DirLight->FrustumEndWorldSpace[0].z,
		//	DirLight->FrustumEndWorldSpace[0].w));
		//CHECK_GL_ERROR(glUniform4f(FrustumShadowEndWorldSpace1Location,
		//	DirLight->FrustumEndWorldSpace[1].x,
		//	DirLight->FrustumEndWorldSpace[1].y,
		//	DirLight->FrustumEndWorldSpace[1].z,
		//	DirLight->FrustumEndWorldSpace[1].w));
		//CHECK_GL_ERROR(glUniform4f(FrustumShadowEndWorldSpace2Location,
		//	DirLight->FrustumEndWorldSpace[2].x,
		//	DirLight->FrustumEndWorldSpace[2].y,
		//	DirLight->FrustumEndWorldSpace[2].z,
		//	DirLight->FrustumEndWorldSpace[2].w));
		//CHECK_GL_ERROR(glUniform4f(FrustumShadowEndWorldSpace3Location,
		//	DirLight->FrustumEndWorldSpace[3].x,
		//	DirLight->FrustumEndWorldSpace[3].y,
		//	DirLight->FrustumEndWorldSpace[3].z,
		//	DirLight->FrustumEndWorldSpace[3].w));

		for (int i = 0; i < 1; i++) // 这里应该是3次循环 每次使用不同的shadowmap来完成对场景的阴影渲染 但是 这里的混合失效了 会造成在finalcolorbuffer中颜色不是混合 变成了叠加 这里还是要注意一下
		{
			BWMatrix4  LightViewMatrix = DirLight->GetShadowMapProjectedInfor(i).ViewMatrix;
			/*CHECK_GL_ERROR(glUniform1i(ShadowIndexLocation, i));

			BWMatrix4  LightProjectionMatrix = DirLight->GetShadowMapProjectedInfor(i).ProjectMatrix;

			GLfloat mat[16];
			for (int i = 0; i < 16; i++)
			mat[i] = LightViewMatrix.M[i];
			GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, mat);
			CHECK_GL_ERROR(glUniformMatrix4fv(LightViewMatrixLocation, 1, false, mat));

			for (int i = 0; i < 16; i++)
			mat[i] = LightProjectionMatrix.M[i];
			GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, mat);
			CHECK_GL_ERROR(glUniformMatrix4fv(LightProjectionMatrixLocation, 1, false, mat));

			GLHardwareDepthBuffer * ShadowMap = dynamic_cast<GLHardwareDepthBuffer*>(DirLight->GetShadowMapProjectedInfor(i).ShadowTexture.Get());
			CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE3));
			CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, ShadowMap->GetHIID()));*/

			/*DirectLightGLSLProgram->SetGPUProgramParameters(DirectLightGLSLPrgramUsage->GetGpuProgramParameter());
			RenderRenderOperation(CubeMeshRenderOperation, LightViewMatrix);*/

			BWHighLevelGpuProgramPtr tmp;
			RenderOperation(CubeMeshRenderOperation, tmp);
		}
		Light++;

	}
	Pipeline.DepthAndStencilState = TStaticDepthAndStencilState<true, true>::GetStateHI();
	Pipeline.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	SetGraphicsPipelineState(Pipeline);
}