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
#include "..\RenderSystem_GL\GLTexture.h"
#include "..\RenderSystem_GL\GLPreDefine.h"
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

	//RenderMotionBlur();


	RenderTemporalAA();
	
	RenderScreenSpaceReflection();

	RenderToneMap();
	//没有tonemap 就要有下面几句
	/*RSGraphicPipelineState PipelineState;
	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<true, true>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);
	*/

	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->EndFrame();


	CopyTextureToScreen(BWRenderSystem::FinalRenderResult, 0, 0);
	ClearTextureResource();
	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BWRenderSystem::FinalRenderResult;
	SetRenderTarget(nullptr, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR);
	//GRenderTarget->clearRenderTarget();  // 使用下面这种方式的时候 可能出现残影 注释掉SSR的时候 这种现象消失 但是 如果使用 GRenderTarget->clearRenderTarget(); 则不会出现任何问题 

	SetupGBufferRenderTarget(ClearGBufferProgramUsage);
	ClearRenderTarget(FBT_COLOUR | FBT_DEPTH | FBT_STENCIL);
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
	RenderTarget.RenderTargetTexture = ABufferTexture;
	RenderTargets.RenderTargets.push_back(RenderTarget);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BBufferTexture;
	RenderTargets.RenderTargets.push_back(RenderTarget);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = CBufferTexture;
	RenderTargets.RenderTargets.push_back(RenderTarget);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = VelocityRT;
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
//void BWRenderSystem::bindGPUProgramUsage(BWGpuProgramUsage*gpuPrgramUsage)
//{
//	assert(0);
//}
bool BWRenderSystem::InitRendererResource()
{
	auto LoadGUPUsageAndGPUProgram = [](std::string &MaterialName, BWGpuProgramUsagePtr& GPUUsage, BWHighLevelGpuProgramPtr& GPUProgram)
	{
		BWMaterialPtr Material = BWMaterialManager::GetInstance()->GetResource(MaterialName, "General");
		if (Material.IsNull())
		{
			Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : Cant Get The  Material");
			return;
		}
		Material->Load();
		GPUUsage = Material->getTechnique(0)->GetPass(0)->getGPUProgramUsage();
		GPUProgram = GPUUsage->GetHighLevelGpuProgram();
		GPUProgram->Load();
	};

	auto Create2DRenderTexture = [](std::string &TextureName , BWTexturePtr& Texture , int Width, int Hieght , PixelFormat Format)
	{
		Texture = BWTextureManager::GetInstance()->Create(TextureName, "General");
		Texture->setHeight(Hieght);
		Texture->setWidth(Width);
		Texture->setTextureType(TEX_TYPE_2D);
		Texture->setFormat(Format);
		Texture->setNumMipmaps(0);
		Texture->SetIndex(0);
		Texture->createInternalResources();
	};
	GRenderTarget = createRenderTarget("GRenderTarget");
	GRenderTarget->setWidth(1024);
	GRenderTarget->setHeight(768);

	int WindowsWidth = 1024;
	int WindowsHeight = 768;
	Create2DRenderTexture(std::string("ABuffer"), ABufferTexture, WindowsWidth, WindowsHeight, PF_FLOAT32_RGBA);
	Create2DRenderTexture(std::string("BBuffer"), BBufferTexture, WindowsWidth, WindowsHeight, PF_FLOAT32_RGBA);
	Create2DRenderTexture(std::string("CBuffer"), CBufferTexture, WindowsWidth, WindowsHeight, PF_FLOAT32_RGBA);
	Create2DRenderTexture(std::string("VelocityRT"), VelocityRT, WindowsWidth, WindowsHeight, PF_FLOAT32_RGBA);
	Create2DRenderTexture(std::string("FinalRenderReslutColor"), FinalRenderResult, WindowsWidth, WindowsHeight, PF_BYTE_RGBA);
	Create2DRenderTexture(std::string("HistoryRT"), HistoryRT, WindowsWidth, WindowsHeight, PF_FLOAT32_RGBA);
	GDepthBuffer = new GLHardwareDepthBuffer("DepthBuffer", WindowsWidth, WindowsHeight, 0, BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
	

	mPointLightMesh = BWMeshManager::GetInstance()->load("sphere.mesh", "General");
	mCubeMesh = BWMeshManager::GetInstance()->load("cube.mesh", "General");
	mCubeMesh->getSubMesh(0)->_getRenderOperation(CubeMeshRenderOperation);
	
	////LoadGUPUsageAndGPUProgram(std::string("Ogre/BaseDirectLightingMaterial"), DirectLightGLSLPrgramUsage, DirectLightGLSLProgram);
	//mPointLightM = BWMaterialManager::GetInstance()->GetResource("PointLightDefferLightting", "General");
	//if (mPointLightM.IsNull())
	//{
	//	Log::GetInstance()->logMessage("BWRenderSystem::InitRendererResource() : cant get the mDirectionLightM material");
	//	return false;
	//}
	AOSamplerTexture = BWTextureManager::GetInstance()->Create("AOSamplerTexture", "General");
	AOSamplerTexture->setWidth(4);
	AOSamplerTexture->setHeight(4);
	AOSamplerTexture->setTextureType(TEX_TYPE_2D);
	AOSamplerTexture->setFormat(PF_FLOAT32_RGB);
	AOSamplerTexture->setNumMipmaps(0);
	std::vector<float> SamplerData;
	auto RandNum = [=] { return float(std::rand()) / RAND_MAX; };
	for (int i = 0; i < 16 * 3; i += 3)
	{
		SamplerData.push_back(RandNum() * 2.0 - 1);
		SamplerData.push_back(RandNum() * 2.0 - 1);
		SamplerData.push_back(0);
	}
	AOSamplerTexture->CreateInternalResourcesWithData(SamplerData.data());

	Create2DRenderTexture(std::string("BloomTexture"), BloomTexture, FinalRenderResult->getWidth() / 2, FinalRenderResult->getHeight() / 2, PF_FLOAT32_RGB);
	Create2DRenderTexture(std::string("ToneMapTexture"), ToneMapTexture, FinalRenderResult->getWidth() / 2, FinalRenderResult->getHeight() / 2, PF_FLOAT32_RGB);
	

	LoadGUPUsageAndGPUProgram(std::string("ScaleCopy"), EmptyGPUProgramUsage, EmptyGPUProgram);
	LoadGUPUsageAndGPUProgram(std::string("ComputeLumValue"), ComputeLumUsage, ComputeLumValue);
	LoadGUPUsageAndGPUProgram(std::string("ComputeBloom"), ComputeBloomUsage, ComputeBloomProgram);
	LoadGUPUsageAndGPUProgram(std::string("ToneMap"), ToneMapProgramUsage, ToneMapProgram);
	LoadGUPUsageAndGPUProgram(std::string("FilterImageGussiX"), FilterImageByGussiXUsage, FilterImageByGussiX);
	LoadGUPUsageAndGPUProgram(std::string("FilterImageGussiY"), FilterImageByGussiYUsage, FilterImageByGussiY);
	LoadGUPUsageAndGPUProgram(std::string("TemporalAntiAliasing"), TemporalAAUsage, TemporalAAProgram);
	LoadGUPUsageAndGPUProgram(std::string("MotionBlur"), MotionBlurUsage,MotionBlurProgram);
	LoadGUPUsageAndGPUProgram(std::string("ScreenSpaceRayTrack"), ScreenSpaceRayTrackProgramUsage, ScreenSpaceRayTrackProgram);
	LoadGUPUsageAndGPUProgram(std::string("ScreenSpaceReflection"), ScreenSpaceReflectionProgramUsage, ScreenSpaceReflectionProgram);
	LoadGUPUsageAndGPUProgram(std::string("ClearDefferDataBuffer"), ClearGBufferProgramUsage, ClearGBufferProgram);
	LoadGUPUsageAndGPUProgram(std::string("AmbientOcclusion"), AmbientOcclusionProgramUage, AmbientOcclusionGPUProgram);
	LoadGUPUsageAndGPUProgram(std::string("Ogre/BaseDirectLightingMaterial"), DirectLightProgramUsage, DirectLightProgram);
	LoadGUPUsageAndGPUProgram(std::string("ImageBaseLighting"), ImageBaseLightingUsage, ImageBaseLighting);
	LoadGUPUsageAndGPUProgram(std::string("SkyBox"), mSkyBoxGpuPrgramUsage, mSkyBoxProgram);

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
		new GLHardwareDepthBuffer("TempRenderTarget", HDRCubeMap->getWidth(), HDRCubeMap->getHeight(), 0,BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
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
	BWTexturePtr GatherSHReslut;

	LoadGUPUsageAndGPUProgram(std::string("SHConvolution"), SHConvolutonUsage, SHConvolution);
	LoadGUPUsageAndGPUProgram(std::string("AccumulateDiffuse"), AccumulateUsage, AccumulateDiffuse);
	LoadGUPUsageAndGPUProgram(std::string("AccumulateReadCubeMap"), AccumulateReadCubeFaceUsage, AccumulateReadCubeFace);
	Create2DRenderTexture(std::string("GatherSHReslut"), GatherSHReslut, SHVector.MaxBasis, 1, PF_FLOAT32_RGBA);

	float Sampler01[4] = { 0.0f ,0.0f ,0.0f, 0.0f };
	float Sampler23[4] = { 0.0f ,0.0f ,0.0f, 0.0f };
	float Mask[TSHVector<3>::MaxBasis];
	const int CubeMapWidth = mProcessEvnMapTexture->getWidth();
	const int CubeMapHigh = mProcessEvnMapTexture->getHeight();
	const int MipLevelNum = HelperFunction::GetMaxMipmaps(CubeMapWidth, CubeMapHigh, 1);
	int CurrentMipLevel = 0;
	BWHardwareDepthBufferPtr SHDepthBuffer =
		new GLHardwareDepthBuffer("TempRenderTarget", CubeMapWidth, CubeMapHigh, 0, BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
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

	for (int i = 0; i < SHVector.MaxBasis; i++)
	{
		CurrentMipLevel = 0;
		for (int k = 0; k < SHVector.MaxBasis; k++)
		{
			Mask[k] = i == k ? 1.0f : 0.0f;
		}
		PipeLineState.GPUProgramUsage = SHConvolutonUsage;
		RenderTarget.RenderTargetTexture = AccumulationCubeMaps[CurrentMipLevel % 2];

		SetGraphicsPipelineState(PipeLineState);
		SetShaderTexture(SHConvolution, HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());
		SetViewport(0, 0, CubeMapWidth, CubeMapHigh);

		for (int CubeFace = 0; CubeFace < 6; CubeFace++)
		{
			RenderTarget.Index = CubeFace;
			RenderTarget.MipmapLevel = CurrentMipLevel;
			SetRenderTarget(SHConvolutonUsage, RenderTarget, TmpDepthBuffer);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("Mask0", Mask, 4, 1);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("Mask1", &Mask[4], 4, 1);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("Mask2", &Mask[8], 1, 1);
			SHConvolutonUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[CubeFace]);
			ClearRenderTarget(FBT_DEPTH);
			BWHighLevelGpuProgramPtr tmp;
			RenderOperation(CubeMeshRenderOperation, tmp);
		}
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
				SetRenderTarget(AccumulateUsage, RenderTarget, TmpDepthBuffer);
				ClearRenderTarget(FBT_DEPTH);
				BWHighLevelGpuProgramPtr tmp;
				RenderOperation(CubeMeshRenderOperation, tmp);
			}
		}
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
		new GLHardwareDepthBuffer("TempRenderTarget1", IBL_Diffuse_Cube_Map->getWidth(), IBL_Diffuse_Cube_Map->getHeight(), 0,BWHardwareBuffer::Usage::HBU_DYNAMIC, false, false);
	TmpDepthBuffer1->SetIsWithStencil(false);




	SetShaderTexture(mProcessEvnMapProgram, HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	SetViewport(0, 0, IBL_Diffuse_Cube_Map->getWidth(), IBL_Diffuse_Cube_Map->getHeight());
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



	LoadGUPUsageAndGPUProgram(std::string("ProcessEnvMapForSpecular"), mProcessEvnMapForSpecularGpuPrgramUsage, mProcessEvnMapForSpecularProgram);

	const int RoughnessLevel = CubemapMaxMip + 1;
	int IBL_Width = 1024;
	int IBL_Height = 1024;
	IBL_Specular_Cube_Map = BWTextureManager::GetInstance()->Create("IBL_Specular_Cube_Map", "General");
	IBL_Specular_Cube_Map->setTextureType(TEX_TYPE_CUBE_MAP);
	IBL_Specular_Cube_Map->setFormat(PF_FLOAT32_RGB);
	IBL_Specular_Cube_Map->setWidth(IBL_Width);
	IBL_Specular_Cube_Map->setHeight(IBL_Height);
	IBL_Specular_Cube_Map->setNumMipmaps(RoughnessLevel);
	IBL_Specular_Cube_Map->createInternalResources();
	PipeLineState.GPUProgramUsage = mProcessEvnMapForSpecularGpuPrgramUsage;
	SetGraphicsPipelineState(PipeLineState);

	int Width = IBL_Specular_Cube_Map->getWidth();
	int Height = IBL_Specular_Cube_Map->getHeight();
	HDRCubeMap->SetIndex(0);
	SetShaderTexture(mProcessEvnMapForSpecularProgram, HDRCubeMap, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	RenderTarget.RenderTargetTexture = IBL_Specular_Cube_Map;
	for (int mip = 0; mip < RoughnessLevel; mip++)
	{
		int MipWidth = Width * std::pow(0.5, mip);
		int MipHeight = Height  * std::pow(0.5, mip);
		SetViewport(0, 0, MipWidth, MipHeight);
		RenderTarget.MipmapLevel = mip;
		float roughness = mip / float(RoughnessLevel - 1);
		TmpDepthBuffer1->SetHardwareBufferSize(MipWidth, MipHeight);
		for (int i = 0; i < 6; i++)
		{
			RenderTarget.Index = i;
			SetRenderTarget(mProcessEvnMapForSpecularGpuPrgramUsage, RenderTarget, TmpDepthBuffer1);
			mProcessEvnMapForSpecularGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("ViewMatrix", ViewMatrixs[i]);
			mProcessEvnMapForSpecularGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("roughness", &roughness, 1, 1);
			ClearRenderTarget(FBT_DEPTH | FBT_COLOUR);
			BWHighLevelGpuProgramPtr tmp;
			RenderOperation(CubeMeshRenderOperation, tmp);
		}
	}
	
	//计算LUT
	LoadGUPUsageAndGPUProgram(std::string("PreprocessEvnMapLUT"), mProcessEvnMapLUTGpuPrgramUsage, mProcessEvnMapLUTProgram);
	Create2DRenderTexture(std::string("IBL_LUT"), IBL_LUT, 512, 512, PF_FLOAT32_RGB);
	SetViewport(0, 0, 512, 512);
	TmpDepthBuffer1->SetHardwareBufferSize(512, 512);
	PipeLineState.GPUProgramUsage = mProcessEvnMapLUTGpuPrgramUsage;
	SetGraphicsPipelineState(PipeLineState);
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = IBL_LUT;
	SetRenderTarget(mProcessEvnMapLUTGpuPrgramUsage, RenderTarget, TmpDepthBuffer1);
	ClearRenderTarget(FBT_DEPTH);
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


void BWRenderSystem::ClearTextureResource()
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

void BWRenderSystem::RenderAmbientOcclusion()
{
	if (!IsEnableSSAO) return;
	RSGraphicPipelineState Pipeline;
	static std::vector<float> SamplerDir;
	if (SamplerDir.size() == 0)
	{
		auto RandNum = [=]{ return  float(std::rand()) / RAND_MAX; };
		for (int i = 0; i < 64 * 3; i+= 3)
		{
			BWVector3 Sample;
			Sample.x = RandNum() * 2.0 - 1;
			Sample.y = RandNum() * 2.0 - 1;
			Sample.z = RandNum();
			Sample.normalize();
			Sample = Sample * RandNum();
			SamplerDir.push_back(Sample.x);
			SamplerDir.push_back(Sample.y);
			SamplerDir.push_back(Sample.z);
		}
	}
	Pipeline.GPUProgramUsage = AmbientOcclusionProgramUage;
	Pipeline.BlendState = TStaticBlendStateHI<true, SBO_ADD, SBF_ZERO, SBF_ONE, true, SBO_ADD, SBF_ONE, SBF_ZERO>::GetStateHI();
	Pipeline.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	Pipeline.RasterizerState = TStaticRasterizerState<PM_SOLID>::GetStateHI();
	SetGraphicsPipelineState(Pipeline);


	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->SetGPUAutoParameter(
		AmbientOcclusionProgramUage->GetGpuProgramParameter()
		);
	AmbientOcclusionProgramUage->GetHighLevelGpuProgram();

	
	BBufferTexture->SetIndex(0);
	AOSamplerTexture->SetIndex(1);
	SetShaderTexture(AmbientOcclusionGPUProgram, BBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(AmbientOcclusionGPUProgram, AOSamplerTexture, TStaticSamplerState<FO_LINEAR, FO_POINT, FO_NONE, SAM_REPEAT , SAM_REPEAT , SAM_REPEAT>::GetStateHI());

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = CBufferTexture;
	SetRenderTarget(AmbientOcclusionProgramUage, RenderTarget, GDepthBuffer);
	
	AmbientOcclusionProgramUage->GetGpuProgramParameter()->SetNamedConstant("SampleDirection", SamplerDir.data(), 64 * 3, 1);

	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);

	// Filter Operation
	Pipeline.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	SetGraphicsPipelineState(Pipeline);
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
    // Image Base Lighting 
	RSGraphicPipelineState PipelineState;
	PipelineState.GPUProgramUsage = ImageBaseLightingUsage;
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	PipelineState.BlendState = TStaticBlendStateHI<true, SBO_ADD, SBF_ONE, SBF_ONE ,true ,SBO_ADD , SBF_ONE , SBF_ONE>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);

	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->SetGPUAutoParameter(
		ImageBaseLightingUsage->GetGpuProgramParameter()
		);
	ImageBaseLighting = ImageBaseLightingUsage->GetHighLevelGpuProgram();
	

	ABufferTexture->SetIndex(0);
	BBufferTexture->SetIndex(1);
	CBufferTexture->SetIndex(2);
	IBL_Specular_Cube_Map->SetIndex(3);
	IBL_LUT->SetIndex(4);

	// 使用一张贴图来进行环境满反射 目前我們使用的是SH
	//IBL_Diffuse_Cube_Map->SetIndex(5);
	//SetShaderTexture(DirectLighting, IBL_Diffuse_Cube_Map, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	SetShaderTexture(ImageBaseLighting, ABufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, BBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, CBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, IBL_Specular_Cube_Map, TStaticSamplerState<FO_POINT, FO_POINT, FO_LINEAR>::GetStateHI());
	SetShaderTexture(ImageBaseLighting, IBL_LUT, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = BWRenderSystem::FinalRenderResult;
	SetRenderTarget(ImageBaseLightingUsage, RenderTarget,GDepthBuffer);
	//Unreal 中只在SkyLighting中使用了SH来模拟  其他IBL只使用了镜面环境光照 这里我们没有使用SH系数
	ImageBaseLightingUsage->GetGpuProgramParameter()->SetNamedConstant("SHCoefficient", SHVector.V, 36, 1);
	ImageBaseLightingUsage->GetGpuProgramParameter()->SetNamedConstant("CubemapMaxMip", &CubemapMaxMip, 1, 1);
	
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);
	
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<true, true>::GetStateHI();
	PipelineState.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);
}


void BWRenderSystem::RenderToneMap()
{
	int Width; 
	int Hight;
	RSGraphicPipelineState PipelineState;
	RSRenderTarget RenderTarget;
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	PipelineState.GPUProgramUsage = EmptyGPUProgramUsage;
	SetGraphicsPipelineState(PipelineState);
	SetViewport(0, 0, ToneMapTexture->getWidth(), ToneMapTexture->getHeight());

	FinalRenderResult->SetIndex(0);
	SetShaderTexture(EmptyGPUProgram, FinalRenderResult, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = ToneMapTexture;
	SetRenderTarget(EmptyGPUProgramUsage ,RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR);
	Width = ToneMapTexture->getWidth();
	Hight = ToneMapTexture->getHeight();
	EmptyGPUProgramUsage->GetGpuProgramParameter()->SetNamedConstant("Width", &Width, 1, 1);
	EmptyGPUProgramUsage->GetGpuProgramParameter()->SetNamedConstant("Hight", &Hight, 1, 1);
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);


	SetViewport(0, 0, BloomTexture->getWidth(), BloomTexture->getHeight());
	PipelineState.GPUProgramUsage = ComputeBloomUsage;
	SetGraphicsPipelineState(PipelineState);
	ToneMapTexture->SetIndex(0);
	SetShaderTexture(ComputeLumValue, ToneMapTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	// 这里只是利用BloomTexture 计算平均亮度
	RenderTarget.RenderTargetTexture = BloomTexture;
	SetRenderTarget(ComputeLumUsage, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR);
	RenderOperation(CubeMeshRenderOperation, tmp);
	int LumDataNum = BloomTexture->getWidth() * BloomTexture->getHeight() * 3;
	float *LumData = new float[LumDataNum];
	BWPixelBox PixelBox(BloomTexture->getWidth(),BloomTexture->getHeight(), 0,PixelFormat::PF_FLOAT32_RGB, LumData);
	ReadSurfaceData(BloomTexture, 0, 0, PixelBox);
	float AvgLum = 0.0f;
	for (int i = 0; i < LumDataNum; i+=3)
	{
		AvgLum += LumData[i];
	}
	delete[] LumData;
	AvgLum = std::exp(AvgLum / LumDataNum);
	static float LumAdapt = 0.0f; // 模拟人眼的自适应
    static float TimeDlta = 0.0;
	//TimeDlta += 0.03; 
	LumAdapt += (AvgLum - LumAdapt) *(1 - std::pow(0.98f, 30.f * TimeDlta));
	InExposeScale = AvgLum;
	//Bloom
	// Get The Bloom Area
	PipelineState.GPUProgramUsage = ComputeBloomUsage;
	SetGraphicsPipelineState(PipelineState);

	ToneMapTexture->SetIndex(0);
	SetShaderTexture(ComputeBloomProgram, ToneMapTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	Width = ToneMapTexture->getWidth();
	Hight = ToneMapTexture->getHeight();
	ComputeBloomUsage->GetGpuProgramParameter()->SetNamedConstant("AvgLum", &AvgLum, 1, 1);
	ComputeBloomUsage->GetGpuProgramParameter()->SetNamedConstant("Width", &Width, 1, 1);
	ComputeBloomUsage->GetGpuProgramParameter()->SetNamedConstant("Hight", &Hight, 1, 1);

	RenderTarget.RenderTargetTexture = BloomTexture;
	SetRenderTarget(ComputeBloomUsage,RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR|FBT_DEPTH);
	RenderOperation(CubeMeshRenderOperation, tmp);

	//Filter Bloom 
	FilterTexture(BloomTexture);


	//ToneMap And Bloom
	static BWTexturePtr DumpToneMapTexture = BWTextureManager::GetInstance()->Create("DumpToneMapTexure", DEFAULT_RESOURCE_GROUP);
	DumpToneMapTexture->setWidth(FinalRenderResult->getWidth());
	DumpToneMapTexture->setHeight(FinalRenderResult->getHeight());
	DumpToneMapTexture->setFormat(FinalRenderResult->getFormat());
	DumpToneMapTexture->setTextureType(TEX_TYPE_2D);
	DumpToneMapTexture->setNumMipmaps(0);
	DumpToneMapTexture->SetIndex(0);
	DumpToneMapTexture->createInternalResources();
	CopyTextureToTexture(FinalRenderResult, 0, 0, DumpToneMapTexture, 0, 0);
	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	PipelineState.GPUProgramUsage = ToneMapProgramUsage;
	SetGraphicsPipelineState(PipelineState);

	DumpToneMapTexture->SetIndex(0);
	BloomTexture->SetIndex(1);
	SetShaderTexture(ToneMapProgram, DumpToneMapTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
    SetShaderTexture(ToneMapProgram, BloomTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	float Key = 1.0f;

	//ToneMapProgramUsage->GetGpuProgramParameter()->SetNamedConstant("AverageLum", &LumAdapt, 1, 1);
	ToneMapProgramUsage->GetGpuProgramParameter()->SetNamedConstant("AvgLum", &AvgLum, 1, 1);
	ToneMapProgramUsage->GetGpuProgramParameter()->SetNamedConstant("Key", &Key, 1, 1);
	ToneMapProgramUsage->GetGpuProgramParameter()->SetNamedConstant("ExposureBias", &ExposureBias, 1, 1);

	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = FinalRenderResult;
	SetRenderTarget(ToneMapProgramUsage ,RenderTarget, GDepthBuffer);
	RenderOperation(CubeMeshRenderOperation, tmp);


	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<true, true>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);
	return;
}


void BWRenderSystem::RenderMotionBlur()
{
	RSGraphicPipelineState CacheCachePipelineState = CachedPipelineState;
	RSGraphicPipelineState PipelineState;
	PipelineState.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	PipelineState.RasterizerState = TStaticRasterizerState<>::GetStateHI();
	PipelineState.GPUProgramUsage = MotionBlurUsage;
	SetGraphicsPipelineState(PipelineState);
	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	FinalRenderResult->SetIndex(0);
	VelocityRT->SetIndex(1);
	SetShaderTexture(MotionBlurProgram, FinalRenderResult, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(MotionBlurProgram, VelocityRT, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	static BWTexturePtr DumpTexture = BWTextureManager::GetInstance()->Create(std::string("DumpTexture"), DEFAULT_RESOURCE_GROUP);
	DumpTexture->setWidth(FinalRenderResult->getWidth());
	DumpTexture->setHeight(FinalRenderResult->getHeight());
	DumpTexture->setFormat(FinalRenderResult->getFormat());
	DumpTexture->setTextureType(TEX_TYPE_2D);
	DumpTexture->setNumMipmaps(0);
	DumpTexture->SetIndex(0);
	DumpTexture->createInternalResources();

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = DumpTexture;
	SetRenderTarget(MotionBlurUsage, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR | FBT_DEPTH);
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);
	CopyTextureToTexture(DumpTexture, 0, 0, FinalRenderResult, 0, 0);
	SetGraphicsPipelineState(CacheCachePipelineState);
	return;

}

void BWRenderSystem::RenderTemporalAA()
{
	if (!IsEnableTemporalAA) return;
	RSGraphicPipelineState PipelineState;
	PipelineState.GPUProgramUsage = TemporalAAUsage;
	PipelineState.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState <false, false>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);
	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->SetGPUAutoParameter(
		ToneMapProgramUsage->GetGpuProgramParameter()//?????
		);
	BBufferTexture->SetIndex(0);
	VelocityRT->SetIndex(1);
	HistoryRT->SetIndex(2);
	FinalRenderResult->SetIndex(3);
	SetShaderTexture(TemporalAAProgram, BBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(TemporalAAProgram, FinalRenderResult, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(TemporalAAProgram, HistoryRT, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(TemporalAAProgram, VelocityRT, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	static BWTexturePtr DumpTexture = BWTextureManager::GetInstance()->Create(std::string("DumpTexture"), DEFAULT_RESOURCE_GROUP);
	DumpTexture->setWidth(HistoryRT->getWidth());
	DumpTexture->setHeight(HistoryRT->getHeight());
	DumpTexture->setFormat(HistoryRT->getFormat());
	DumpTexture->setTextureType(TEX_TYPE_2D);
	DumpTexture->setNumMipmaps(0);
	DumpTexture->SetIndex(0);
	DumpTexture->createInternalResources();

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = DumpTexture;
	SetRenderTarget( TemporalAAUsage, RenderTarget, GDepthBuffer);
	// 这些本来使用Unreal中的算法的 后来改成了另一种更简单的方式 但是可以看看Unreal中相关的算法中使用的技巧
	const float JitterX = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->GetTemporalAAJitterX();
	const float JitterY = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->GetTemporalAAJitterY();
	static const float SampleOffsets[9][2] =
	{
		{ -1.0f, -1.0f },
		{ 0.0f, -1.0f },
		{ 1.0f, -1.0f },
		{ -1.0f,  0.0f },
		{ 0.0f,  0.0f },
		{ 1.0f,  0.0f },
		{ -1.0f,  1.0f },
		{ 0.0f,  1.0f },
		{ 1.0f,  1.0f },
	};
	float Weights[9];
	float WeightsPlus[5];
	float TotalWeight = 0.0f;
	float TotalWeightLow = 0.0f;
	float TotalWeightPlus = 0.0f;
	float FilterSize = 1.0;
	int Width = FinalRenderResult->getWidth();
	int Height = FinalRenderResult->getHeight();

	for (int i = 0; i < 9; i++)
	{
		float PixelOffsetX = SampleOffsets[i][0] - JitterX;
		float PixelOffsetY = SampleOffsets[i][1] - JitterY;
		
		PixelOffsetX /= FilterSize;
		PixelOffsetY /= FilterSize;

		Weights[i] = exp(-2.29f * (PixelOffsetX * PixelOffsetX + PixelOffsetY * PixelOffsetY));
		TotalWeight += Weights[i];
	}

	WeightsPlus[0] = Weights[1];
	WeightsPlus[1] = Weights[3];
	WeightsPlus[2] = Weights[4];
	WeightsPlus[3] = Weights[5];
	WeightsPlus[4] = Weights[7];
	TotalWeightPlus = Weights[1] + Weights[3] + Weights[4] + Weights[5] + Weights[7];
	for (int i = 0; i < 5; i++) WeightsPlus[i] /= TotalWeightPlus;
	TemporalAAUsage->GetGpuProgramParameter()->SetNamedConstant("PlusWeights", WeightsPlus, 5, 1);
	TemporalAAUsage->GetGpuProgramParameter()->SetNamedConstant("Width", &Width, 1, 1);
	TemporalAAUsage->GetGpuProgramParameter()->SetNamedConstant("Height", &Height, 1, 1);
	TemporalAAUsage->GetGpuProgramParameter()->SetNamedConstant("InExposureScale", &InExposeScale, 1, 1);
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);
	CopyTextureToTexture(DumpTexture, 0, 0, HistoryRT, 0, 0);// 其实可以用DunmpTexture来代替HistoryRT
	CopyTextureToTexture(DumpTexture, 0, 0, FinalRenderResult, 0, 0);
}


void BWRenderSystem::RenderScreenSpaceReflection()
{
	if (!IsEnableSSR) return;
	RSGraphicPipelineState PipelineState;
	PipelineState.GPUProgramUsage = ScreenSpaceRayTrackProgramUsage;
	PipelineState.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState <false, false>::GetStateHI();
	SetGraphicsPipelineState(PipelineState);
	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->
		SetGPUAutoParameter(
		ScreenSpaceRayTrackProgramUsage->GetGpuProgramParameter()
		);
	BBufferTexture->SetIndex(0);
	FinalRenderResult->SetIndex(1);
	SetShaderTexture(ScreenSpaceRayTrackProgram, BBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ScreenSpaceRayTrackProgram, FinalRenderResult, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	static BWTexturePtr DumpTexture = BWTextureManager::GetInstance()->Create(std::string("DumpTexture"), DEFAULT_RESOURCE_GROUP);
	DumpTexture->setWidth(FinalRenderResult->getWidth());
	DumpTexture->setHeight(FinalRenderResult->getHeight());
	DumpTexture->setFormat(FinalRenderResult->getFormat());
	DumpTexture->setTextureType(TEX_TYPE_2D);
	DumpTexture->setNumMipmaps(0);
	DumpTexture->SetIndex(0);
	DumpTexture->createInternalResources();

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = DumpTexture;

	float StrideZCutoff = 600.f - 1.f;
	float Thickness = 0.1f;
	float MaxDistance = 60000.0f;
	float StrideInPixel = 8.0f;
	float MaxStepCount = 700.0f;
	ScreenSpaceRayTrackProgramUsage->GetGpuProgramParameter()->SetNamedConstant("StrideZCutoff", &StrideZCutoff, 1, 1);
	ScreenSpaceRayTrackProgramUsage->GetGpuProgramParameter()->SetNamedConstant("Thickness", &Thickness, 1, 1);
	ScreenSpaceRayTrackProgramUsage->GetGpuProgramParameter()->SetNamedConstant("MaxDistance", &MaxDistance, 1, 1);
	ScreenSpaceRayTrackProgramUsage->GetGpuProgramParameter()->SetNamedConstant("StrideInPixel", &StrideInPixel, 1, 1);
	ScreenSpaceRayTrackProgramUsage->GetGpuProgramParameter()->SetNamedConstant("MaxStepCount", &MaxStepCount, 1, 1);


	SetRenderTarget(ScreenSpaceRayTrackProgramUsage, RenderTarget, GDepthBuffer);
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);

	
	const float MipmapLevel = HelperFunction::GetMaxMipmaps(FinalRenderResult->getWidth(), FinalRenderResult->getHeight(), 1);
	static BWTexturePtr DumpTextureForScale = BWTextureManager::GetInstance()->Create(std::string("DumpTextureForScale"), DEFAULT_RESOURCE_GROUP);
	DumpTextureForScale->setWidth(FinalRenderResult->getWidth());
	DumpTextureForScale->setHeight(FinalRenderResult->getHeight());// 2 的整数次幂
	DumpTextureForScale->setFormat(FinalRenderResult->getFormat());
	DumpTextureForScale->setTextureType(TEX_TYPE_2D);
	DumpTextureForScale->setNumMipmaps(MipmapLevel - 1);
	DumpTextureForScale->SetIndex(0);
	DumpTextureForScale->createInternalResources();

	//生成Mipmap有问题
	CopyTextureToTexture(FinalRenderResult, 0, 0, DumpTextureForScale, 0, 0);
	//ScaleCopy(FinalRenderResult,DumpTextureForScale, 0);
	//GenerateTextureFilterdMipMap(DumpTextureForScale);
	

	
	PipelineState.GPUProgramUsage = ScreenSpaceReflectionProgramUsage;
	SetGraphicsPipelineState(PipelineState);
	SetViewport(0, 0, FinalRenderResult->getWidth(), FinalRenderResult->getHeight());
	BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->
		SetGPUAutoParameter(
			ScreenSpaceReflectionProgramUsage->GetGpuProgramParameter()
			);
	ABufferTexture->SetIndex(0);
	BBufferTexture->SetIndex(1);
	CBufferTexture->SetIndex(2);
	DumpTextureForScale->SetIndex(3);
	DumpTexture->SetIndex(4);

	SetShaderTexture(ScreenSpaceReflectionProgram, ABufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ScreenSpaceReflectionProgram, BBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ScreenSpaceReflectionProgram, CBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ScreenSpaceReflectionProgram, DumpTextureForScale, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(ScreenSpaceReflectionProgram, DumpTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());

	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = FinalRenderResult;

	float FadeEnd = 0.2;
	float FadeStart = 0.2;
	ScreenSpaceReflectionProgramUsage->GetGpuProgramParameter()->SetNamedConstant("MipLevelNum", &MipmapLevel, 1, 1);
	ScreenSpaceReflectionProgramUsage->GetGpuProgramParameter()->SetNamedConstant("FadeEnd", &FadeEnd, 1, 1);
	ScreenSpaceReflectionProgramUsage->GetGpuProgramParameter()->SetNamedConstant("FadeStart", &FadeStart, 1, 1);

	SetRenderTarget(ScreenSpaceReflectionProgramUsage, RenderTarget, GDepthBuffer);
	RenderOperation(CubeMeshRenderOperation, tmp);



	CopyTextureToTexture(FinalRenderResult, 0, 0, HistoryRT, 0, 0);// 其实可以用DunmpTexture来代替HistoryRT


	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = DumpTextureForScale;
	SetRenderTarget(nullptr, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR | FBT_DEPTH);
	RenderTarget.RenderTargetTexture = DumpTexture;
	SetRenderTarget(nullptr, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR | FBT_DEPTH);
	//CopyTextureToTexture(DumpTexture, 0, 0, HistoryRT, 0, 0);// 其实可以用DunmpTexture来代替HistoryRT
	//CopyTextureToTexture(DumpTexture, 0, 0, FinalRenderResult, 0, 0);
	//CopyTextureToTexture(DumpTextureForScale, 0, 0, FinalRenderResult, 0, 0);
}

void BWRenderSystem::FilterTexture(BWTexturePtr SourceImage)
{
	int Width = SourceImage->getWidth();
	int Hight = SourceImage->getHeight();
	RSGraphicPipelineState CacheCachePipelineState = CachedPipelineState;
	RSGraphicPipelineState PipelineState;
	PipelineState.BlendState = TStaticBlendStateHI<false>::GetStateHI();
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	PipelineState.RasterizerState = TStaticRasterizerState<>::GetStateHI();
	PipelineState.GPUProgramUsage = FilterImageByGussiXUsage;
	SetGraphicsPipelineState(PipelineState);

	SetViewport(0, 0, SourceImage->getWidth(), SourceImage->getHeight());
	SourceImage->SetIndex(0);
	SetShaderTexture(FilterImageByGussiX, SourceImage, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	static BWTexturePtr DumpTexture = BWTextureManager::GetInstance()->Create(std::string("DumpTextureForFilter"), DEFAULT_RESOURCE_GROUP);
	DumpTexture->setWidth(SourceImage->getWidth());
	DumpTexture->setHeight(SourceImage->getHeight());
	DumpTexture->setFormat(SourceImage->getFormat());
	DumpTexture->setTextureType(TEX_TYPE_2D);
	DumpTexture->setNumMipmaps(0);
	DumpTexture->SetIndex(0);
	DumpTexture->createInternalResources();

	RSRenderTarget RenderTarget;
	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = 0;
	RenderTarget.RenderTargetTexture = DumpTexture;
	SetRenderTarget(FilterImageByGussiXUsage, RenderTarget, GDepthBuffer);
	FilterImageByGussiXUsage->GetGpuProgramParameter()->SetNamedConstant("Width", &Width, 1 , 1);
	ClearRenderTarget(FBT_COLOUR | FBT_DEPTH);
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);

	PipelineState.GPUProgramUsage = FilterImageByGussiYUsage;
	SetGraphicsPipelineState(PipelineState);
	DumpTexture->SetIndex(0);
	SetShaderTexture(FilterImageByGussiY, DumpTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	RenderTarget.RenderTargetTexture = SourceImage;
	SetRenderTarget(FilterImageByGussiYUsage, RenderTarget, GDepthBuffer);
	FilterImageByGussiYUsage->GetGpuProgramParameter()->SetNamedConstant("Hight", &Hight, 1, 1);
	ClearRenderTarget(FBT_COLOUR | FBT_DEPTH);
	RenderOperation(CubeMeshRenderOperation, tmp);

	SetGraphicsPipelineState(CacheCachePipelineState);
	return;
}


void BWRenderSystem::ScaleCopy(BWTexturePtr From, BWTexturePtr To , int MipmapLevel)
{
	int Width;
	int Hight;
	RSGraphicPipelineState PipelineState;
	RSRenderTarget RenderTarget;
	PipelineState.DepthAndStencilState = TStaticDepthAndStencilState<false, false>::GetStateHI();
	PipelineState.GPUProgramUsage = EmptyGPUProgramUsage;
	SetGraphicsPipelineState(PipelineState);
	SetViewport(0, 0, To->getWidth(), To->getHeight());

	From->SetIndex(0);
	SetShaderTexture(EmptyGPUProgram, From, TStaticSamplerState<FO_LINEAR>::GetStateHI());


	RenderTarget.Index = 0;
	RenderTarget.MipmapLevel = MipmapLevel;
	RenderTarget.RenderTargetTexture = To;
	// 可能是 GDepthBuffer 大小不对造成的渲染不正确
	SetRenderTarget(EmptyGPUProgramUsage, RenderTarget, GDepthBuffer);
	ClearRenderTarget(FBT_COLOUR);
	Width = From->getWidth() ;
	Hight = From->getHeight();
	EmptyGPUProgramUsage->GetGpuProgramParameter()->SetNamedConstant("Width", &Width, 1, 1);
	EmptyGPUProgramUsage->GetGpuProgramParameter()->SetNamedConstant("Hight", &Hight, 1, 1);
	BWHighLevelGpuProgramPtr tmp;
	RenderOperation(CubeMeshRenderOperation, tmp);
}

void BWRenderSystem::GenerateTextureFilterdMipMap(BWTexturePtr SourceImage)
{
	int Width = SourceImage->getWidth();
	int Height = SourceImage->getHeight();
	const int MipLevelNum = HelperFunction::GetMaxMipmaps(Width, Height, 1);

	static BWTexturePtr DumpTexture = BWTextureManager::GetInstance()->Create(std::string("DumpTextureForMipMap"), DEFAULT_RESOURCE_GROUP);
	DumpTexture->setWidth(SourceImage->getWidth());
	DumpTexture->setHeight(SourceImage->getHeight());
	DumpTexture->setFormat(SourceImage->getFormat());
	DumpTexture->setTextureType(TEX_TYPE_2D);
	DumpTexture->setNumMipmaps(0);
	DumpTexture->createInternalResources();
	CopyTextureToTexture(SourceImage, 0, 0, DumpTexture, 0, 0);
	for (int i = 1; i < MipLevelNum ; i++)
	{
		ScaleCopy(DumpTexture, SourceImage, i);
	}
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

	ABufferTexture->SetIndex(0);
	BBufferTexture->SetIndex(1);
	CBufferTexture->SetIndex(2);
	SetShaderTexture(DirectLighting, ABufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, BBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	SetShaderTexture(DirectLighting, CBufferTexture, TStaticSamplerState<FO_LINEAR>::GetStateHI());
	
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