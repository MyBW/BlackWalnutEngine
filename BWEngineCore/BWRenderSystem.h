#ifndef RENDERSYSTEM_H_
#define RENDERSYSTEM_H_
#include "AllSmartPointRef.h"
#include "BWTextureManager.h"
#include "BWRenderTarget.h"
#include "BWTextureUnitState.h"
#include "BWRenderOperation.h"
#include "AllSmartPointRef.h"
#include "BWDirectionalLight.h"
#include"BWMesh.h"
#include "BWRenderState.h"
#include "BWImageTextureBuffer.h"
#include "GPUProgramType.h"
/////////////////////////// New Interface
struct RSShaderTexture
{
	BWHighLevelGpuProgramPtr GPUProgramUsage;
	BWTexturePtr Texture;
	SamplerStateHIRef Sampler;
};

struct RSRenderTarget
{
	int Index;  // 如果是纹理数组 或者 立方体纹理  表示的位置
	int MipmapLevel;
	BWTexturePtr RenderTargetTexture;
	BWTexturePtr ShaderableTexture;

	int DepthMipmapLevel;
	BWHardwareDepthBufferPtr DepthBuffer;
};

struct RSRenderTargets
{
	BWGpuProgramUsagePtr GPUProgramUsage;
	std::vector<RSRenderTarget> RenderTargets;
	BWHardwareDepthBufferPtr DepthAndStencil;
};

struct RSGraphicPipelineState
{
	RasterizerStateHIRef RasterizerState;
	BlendStateHIRef BlendState;
	DepthAndStencilStateHIRef DepthAndStencilState;
	ColorMaskStateHIRef ColorMaskState;
	//BWHighLevelGpuProgramPtr GPUProgram;
	BWGpuProgramUsagePtr GPUProgramUsage;
};
///////////////////////////////New Interface End
class BWRenderWindow;
class BWViewport;
class BWUniformBufferObject;
enum TexCoordCalcMethod
{
	TEXCALC_NONE ,
	TEXCALC_ENVIRONMENT_MAP , 
	TEXCALC_ENVIRONMENT_MAP_PLANAR ,
	TEXCALC_ENVIRONMENT_MAP_REFLECTION ,
	TEXCALC_ENVIRONMENT_MAP_NORMAL ,
	TEXCALC_PROJECTION_TEXTURE
};
class BWRenderSystem
{
	friend class BWRoot;
	friend class BWAutoParamDataSource;
	friend class BWSceneManager;
	friend class BWFrustum;
	typedef std::map<std::string, BWRenderTarget*> RenderTargetMap;
	typedef std::multimap<unsigned char, BWRenderTarget*> RenderTargetPriorityMap;

public:
	BWRenderSystem();
	virtual ~BWRenderSystem();
	static BWRenderSystem* GetInstance();
	bool hasCapability() { return true; }
	bool getWaitForVerticalBlank();
	void setAmbientLight(float r, float g, float b);
	void _disableTextureUnitsFrom(size_t texUnit);
	virtual bool  isGpuProgramBound(GpuProgramType GPT);
	virtual void  unbindGpuProgram(GpuProgramType GPT);
	virtual void bindGpuProgram(BWGpuProgram *GpuProgamr);
    virtual void clearFrameBuffer(unsigned int buffers, const ColourValue &color = ColourValue::Black, float depth = 1.0, unsigned short stencil = 0) = 0;
	virtual void setNormaliseNormals(bool enbale) const = 0;
	virtual void setCurrentPassIterationCount(int count) { mCurrentPassIterationCount = count; }
	virtual void setShadingType(ShadeOptions type) = 0;
	void attacheRenderTarget(BWRenderTarget *target);
	virtual void initialiseFromRenderSystemCapabilities() = 0;
	virtual void _setViewport(BWViewport *viewport) = 0;
	void setWaitForVerticalBlank(bool enable);
	bool updatePassIterationRenderState();
	virtual BWRenderWindow* _createRenderWindow(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList* miscParam = 0) = 0;
	virtual BWRenderTarget* createRenderTarget(const std::string &name) = 0;

	//virtual void bindGPUProgramUsage(BWGpuProgramUsage*gpuPrgramUsage);



	
	virtual bool InitRendererResource();
	
	
	BWRenderTarget* getGBuffer() { return GRenderTarget; }
	BWRenderTarget* GetShadowMapRenderTarget() { return ShadowMapTarget; }
	BWMeshPrt getPointLightMesh() { return mPointLightMesh; }
	BWMeshPrt GetCubeMesh() { return mCubeMesh; }
	
	void SetGlobalDataSet(BWAutoParamDataSource *GlobalData) { this->GlobalData = GlobalData; }
////////////////////////////////////////// The New Interface
	virtual void SetViewport(int ViewportX , int ViewportY , int ViewportWidth, int ViewportHight);
	virtual void SetScissor(bool IsEnable, int ScissorX = 0, int ScissorY = 0, int ScissorWidth = 0, int ScissorHigh = 0);
	virtual void SetRenderTarget(BWGpuProgramUsagePtr GPUProgramUsage, RSRenderTarget& InRenderTarget, BWHardwareDepthBufferPtr DepthBuffer) { };
	virtual void SetRenderTarget(BWGpuProgramUsagePtr GPUProgramUsage, RSRenderTarget& InRenderTarget){ }
	virtual void SetRenderTargets(RSRenderTargets& InRenderTargets) {};
	virtual BWUniformBufferObject* CreateUniformBufferObject(const std::string &Name) { return NULL; }
	virtual void SetUniformBufferObejct(BWUniformBufferObject* UniformBufferObject, int BindPoint);
	virtual void SetGraphicsPipelineState(RSGraphicPipelineState& InPipelineState);
	virtual void SetShaderTexture(BWHighLevelGpuProgramPtr GPUProgram, BWTexturePtr Texture, SamplerStateHIRef Sampler);
	virtual void SetShaderImageTexture(BWHighLevelGpuProgramPtr GPUProgram, BWImageTexturebufferPtr ImageTexture, int MipLevel, PixelFormat Format);
	virtual void SetShaderImageTexture(BWHighLevelGpuProgramPtr GPUProgram, BWTexturePtr Texture, int Miplevel, PixelFormat Format);
	
	virtual void ClearRenderTarget(unsigned int buffers, const ColourValue &color = ColourValue::Black, float depth = 1.0, unsigned short stencil = 0);
	virtual void ReadSurfaceData(BWTexturePtr SourceInterface, int Index, int MipLevel, BWPixelBox& Destination) { };
	virtual void CopyTextureToTexture(BWTexturePtr SourceTexture, int SourceIndex, int SourceMipmipLevel, BWTexturePtr DestinationTexture, int DestinationIndex, int DestinationMipmapLevel) { }
	virtual void CopyTextureToScreen(BWTexturePtr SourceTexture, int SourceIndex, int SourceMipmipLevel) { };
	virtual void ClearResourceForRender();
	virtual RasterizerStateHIRef CreateRasterizerStateHI(RasterStateInitializer& Initializer) { return nullptr; }
	virtual DepthAndStencilStateHIRef CreateDepthAndStencilHI(DepthAndStencilInitializer& Initializer) { return nullptr; }
	virtual SamplerStateHIRef CreateSamplerStateHI(StaticSamplerStateInitializer& Initializer) { return nullptr; }
	virtual BlendStateHIRef CreateBlendStateHI(StaticBlendStateInitializer& Initializer) { return  nullptr; }
	virtual ColorMaskStateHIRef CreateColorMaskState(StaticColorMaskStateInitializer& Initializer) { return nullptr; }
	virtual void RenderOperation(const BWRenderOperation &RenderOperation, BWHighLevelGpuProgramPtr GPUProgram) { };
	
protected:
	RSGraphicPipelineState CachedPipelineState;
	RSRenderTarget CacheRenderTarget;
	BWVector4 ViewportSize;
	std::vector<RSShaderTexture> ShaderTextures;
	TSHVector<3> SHVector;
	BWRenderOperation CubeMeshRenderOperation;

	BWHighLevelGpuProgramPtr ImageBaseLighting;
	BWGpuProgramUsagePtr ImageBaseLightingUsage;
	
	BWTexturePtr BloomTexture;
	BWTexturePtr ToneMapTexture;
	BWHighLevelGpuProgramPtr DownSampleGPUProgram;
	BWGpuProgramUsagePtr DownSampleGPUProgramUsage;
	BWHighLevelGpuProgramPtr ComputeLumValue;
	BWGpuProgramUsagePtr ComputeLumUsage;
	BWHighLevelGpuProgramPtr ComputeBloomProgram;
	BWGpuProgramUsagePtr ComputeBloomUsage;
	BWHighLevelGpuProgramPtr ToneMapProgram;
	BWGpuProgramUsagePtr ToneMapProgramUsage;

	BWHighLevelGpuProgramPtr FilterImageByGussiX;
	BWGpuProgramUsagePtr FilterImageByGussiXUsage;
	BWHighLevelGpuProgramPtr FilterImageByGussiY;
	BWGpuProgramUsagePtr FilterImageByGussiYUsage;
	
	BWHighLevelGpuProgramPtr MotionBlurProgram;
	BWGpuProgramUsagePtr MotionBlurUsage;
	//Templore AA
	BWTexturePtr HistoryRT;
	BWTexturePtr VelocityRT;
	BWHighLevelGpuProgramPtr TemporalAAProgram;
	BWGpuProgramUsagePtr TemporalAAUsage;
	//Screen Space Reflection
	BWHighLevelGpuProgramPtr ScreenSpaceRayTrackProgram;
	BWGpuProgramUsagePtr ScreenSpaceRayTrackProgramUsage;
	BWHighLevelGpuProgramPtr ScreenSpaceReflectionProgram;
	BWGpuProgramUsagePtr ScreenSpaceReflectionProgramUsage;
	BWHighLevelGpuProgramPtr ClearGBufferProgram;
	BWGpuProgramUsagePtr ClearGBufferProgramUsage;


	void SetupGBufferRenderTarget(BWGpuProgramUsagePtr GPUUsage);

	// 目前出现的问题是在Tonemap过程中最后一步中调用SetShaderTexture会影响到后面的各种纹理调用
	// 所以先用这个函数清理一下资源  随后再考虑是哪里出现了问题
	virtual void ClearTextureResource();


public:
	bool IsEnableSparseVexelGI{ false };
	bool IsEnableTemporalAA{ true };
	bool IsEnableToneMap{ true };
	bool IsEnableSSR{ true };
	bool IsEnableSSAO{ true };
	bool IsEnableIndirectLight{ true };
	bool IsEnableSkyBox{ true };
	bool IsEnableDirectLight{ true };
	bool IsDebugSVO{ false };
	float InExposeScale{ 0.0f };
	float CubemapMaxMip{ 6.0f };
	float ExposureBias{ 4.0f };
	float TonemapKey{ 1.0f };

	float TempDeltal{ 0.0f };
//////////////////////////////////////////
protected:
	// 全局数据
	BWAutoParamDataSource *GlobalData{ nullptr };

	virtual BWRenderWindow* _initialise(bool autoCreateWindow, const std::string &windowTitle) ;
	void _beginFrame();
	virtual void _endFrame();
	unsigned int _getFaceNum();
	unsigned int _getBathcesNum();
	virtual void _setPolygonMode(PolygonMode polygonMode) = 0;
	void _setTextureProjectionRelativeTo(bool camraRelativeRendering, BWVector3 pos);
	void _initRenderTargets();
	void _updateAllRenderTargets(bool swapBuffer = true);
	void _swapAllRenderTargetBuffers(bool waitForVSync);
	// 不同的渲染系统对矩阵左右手要求不一样
	virtual void _convertProjectionMatrix(const BWMatrix4& matrix, BWMatrix4 &dest, bool forGpuProgram = false) = 0;
	void _beginGeometryCount();
	CullingMode _getCullingMode() const;
	
	virtual void _setTexture(size_t texUnit, bool enable, const BWTexturePtr &textPtr) = 0;
	void _setTextureUnitSetting(size_t texUnit, BWTextureUnitState& textureUnit);
	virtual void _setTextureCoordSet(size_t texUnit, unsigned int textureCoordSet) = 0;
	virtual void _setTextureUnitFiltering(size_t texUnit, FilterOptions min, FilterOptions max, FilterOptions mip);
	virtual void _setTextureUnitFiltering(size_t texUnit, FilterType type, FilterOptions mip) = 0;
	virtual void _setTexturelayerAnisotropy(size_t texUnit, unsigned int  anisotropy) = 0;
	virtual void _setCullingMode(CullingMode cullingMode) = 0;
	virtual void _setColourBufferEnable(bool r, bool g, bool b, bool a) = 0;

	virtual void _setAlphaRejectSetting(CompareFunction rejectFunction, unsigned char rejectValue, bool alphToCoverage) = 0;

	//用来处理z fight  和 stiching
	virtual void _setDepthBias(float deptBias, float deptBiaseSlopScale) = 0;
	virtual void _setDepthBufferWriteEnable(bool enbale) = 0;
	virtual void _setDepthBufferCheckEnable(bool enable) = 0;
	virtual void _setDepthBufferFunction(CompareFunction depthFunction) = 0;

	// 这里的混合模式设定的是在已有像素上混合另一个渲染结果的模式
	virtual void _setSeparatesceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor,
		SceneBlendFactor sourceAlphaFactor, SceneBlendFactor destAlphaFactor, SceneBlendOperation op = SBO_ADD, SceneBlendOperation alphaOp = SBO_ADD) = 0;
	virtual void _setSceneBlending(SceneBlendFactor sourceFactro, SceneBlendFactor destFactor, SceneBlendOperation op) = 0;

	 // 衰减方程为 attenuation = 1.0/(constant + linear *dist + quadratic * dist^2)
	virtual void _setPointParameters(float size, bool attenuationEnable, float constant, float linear, float quadratic, float minSize, float maxSize) = 0;
	virtual void _setPointSpriteEnable(bool enable) = 0;

	//设定纹理单元（不是纹理）的值 纹理单元包含了如何产生纹理坐标  纹理之间的混合方式  以及纹理矩阵等
	virtual void _setTextureMipmapBias(size_t texNum, float bias) = 0;
	virtual void _setTextureBlendMode(size_t texNum,const LayerBlendModeEx &model) = 0;
	virtual void _setTextureBorderColour(size_t texNum, const ColourValue &color) = 0;
	virtual void _setTextureAddressionMode(size_t texNum, const BWTextureUnitState::UVWAddressingMode &uvw) = 0;
	virtual void _setTextureCoordCalculation(size_t texNum, TexCoordCalcMethod texCoordCalcMethod , const BWFrustum * frustum = NULL) = 0;
	virtual void _setTextureMatrix(size_t texNum, const BWMatrix4 &matrix) = 0;

	virtual void _setWorldMatrices(const BWMatrix4* m, unsigned short count);
	virtual void _setWorldMatrix(const BWMatrix4 &m) = 0;
	virtual void _setViewMatrix(const BWMatrix4 &m) = 0;
	virtual void _setProjectionMatrix(const BWMatrix4 & matrix) = 0;

	virtual void _render(const BWRenderOperation &ro) ;
	virtual void _setRenderTarget(BWRenderTarget *renderTarget) = 0;
	BWTextureManager * mTextureManager;

	void _disableTextureUnit(size_t texUnit);
	virtual void _setVertexTexture(size_t texUnit,const BWTexturePtr &texture);

	static BWRenderSystem* instance;
	size_t mDisableTexUnitFrom;
	CullingMode mCullingMode;
	RenderTargetMap mRenderTargetMap;
	RenderTargetPriorityMap mPrioritiseRenderTargets;

	size_t mCurrentPassIterationCount; // 当前pass要迭代的次数
	size_t mCurrentPassIterationNum; // 当前已经迭代的次数
	size_t mFaceCount;
	size_t mVertexCount;
	size_t mBatchCount;  // 渲染批次 也就是迭代次数

	bool mDerivedDepthBisa; // 是否每次render的调用都更新depth bias
	float mDerivedDepthBisaBase;
	float mDerivedDepthBisaMultiplier;
	float mDerivedDepthBisaSlopeScale;

	//PlaneList mClipPlanes;
	bool mClipPlanesDirty;
	BWVector3 mTexProjRelativeOrigin;
	BWRenderTarget *mActiveRenderTarget;
	BWViewport *mActiveVireport;
	bool mInvertVertexWinding;
	bool mTexProjRelative;
	ColourValue mManualBlendColours[OGRE_MAX_TEXTURE_LAYERS][2];

	bool mVSync;
	bool mVertexProgramBound;
	bool mGeometryProgramBound;
	bool mFragmentProgramBound;
	bool mUseCustomCapabilities;


	// 延迟渲染相关
	BWRenderTarget* GRenderTarget;

	BWMaterialPtr mDirectionLightM;
	BWGpuProgramUsagePtr  DirectLightProgramUsage;
	BWHighLevelGpuProgramPtr DirectLightProgram;

	BWMeshPrt mCubeMesh;
	BWMaterialPtr mPointLightM;
	BWMeshPrt mPointLightMesh;
	
	BWTexturePtr ABufferTexture;
	BWTexturePtr BBufferTexture;
	BWTexturePtr CBufferTexture;
	BWHardwareDepthBufferPtr GDepthBuffer;
	BWTexturePtr FinalRenderResult;
	std::vector<BWTexturePtr> HolderTexturesFor2D;
	BWTexturePtr Texure2DForFilter;

	BWMaterialPtr AmbientOcclusionMaterial;
	BWTexturePtr AOSamplerTexture;
	BWGpuProgramUsagePtr AmbientOcclusionProgramUage;
	BWHighLevelGpuProgramPtr AmbientOcclusionGPUProgram;
	BWGpuProgramUsagePtr AmbientOcculusioinFilterProgramUsage;
	BWHighLevelGpuProgramPtr AmbientOcculusionFilterGPUProgram;

	BWRenderTarget* ShadowMapTarget;
	BWMaterialPtr DirectionLightShadowMapM;
	BWMaterialPtr PointLightShadowMapM;

	///////////// Global Illumination Base Sparse Voxel Octree

	void InitSparseVoxelOctreeGI();
	void DynamicGenerateVoxel();
	void ShadingSceneWithVoxel();
	virtual void RenderVoxelForDebug();
	void RenderGIWithSparseVoxelOctree();
	
	//////Test For Spare Volex 
	BWVector3 PointLightPos;
	BWVector3 PointLightColor;
	BWVector3 SceneSizeMin;
	BWVector3 SceneSizeMax;
	BWVector4 VoxelSize;
	BWTexturePtr TextureForVoxelizationA;
	BWTexturePtr TextureForVoxelizationB;
	BWTexturePtr TextureForVoxelizationC;
	BWGpuProgramUsagePtr DynamicVoxelSceneProgramUsage;
	BWHighLevelGpuProgramPtr DynamicVoxelSceneProgram;
	BWGpuProgramUsagePtr DynamicVoxelSceneWithoutTextureProgramUsage;
	BWHighLevelGpuProgramPtr DynamicVoxelSceneWithoutTextureProgram;

	BWGpuProgramUsagePtr RenderVoxelForDebugProgramUsage;
	BWHighLevelGpuProgramPtr RenderVoxelForDebugProgram;

	BWGpuProgramUsagePtr ShadingSceneWithVoxelProgramUsage;
	BWHighLevelGpuProgramPtr ShadingSceneWithVoxelProgram;
	//// tmp code 
	std::vector<BWRenderOperation> AllRendererdOparation;
	std::vector<const BWPass*> AllPass;
	std::vector<BWMatrix4> AllRenderOparationWorldMatrix;
	//////////// Global Illumination Base Sparse Voxel Octree End

	void RenderAmbientOcclusion() ;
	void RenderLights();
	void RenderInDirectLights();
	void RenderToneMap();
	void RenderMotionBlur();
	void RenderTemporalAA();
	void RenderScreenSpaceReflection();

	template<bool Red, bool Green, bool Blue, bool Alpha>
	void FilterTexture(BWTexturePtr SourceImage);
	void ProcessTexture(BWTexturePtr SourceTexture, BWTexturePtr ReslutTexture, int ReslutMipLevel, RSGraphicPipelineState& PipelineState);
	void GenerateTextureFiltedMipMap(BWTexturePtr SourceImage);
	virtual void RenderLightsShadowMaps();
	virtual void FinishLightsShadowMaps() {}
	virtual void SetRenderShadowMapRenderTarget() { };

	virtual void SetLightPreparForRenderShadow(BWDirectionalLight * DirectionalLight) { };
	virtual void RemoveLightFromRenderShadow(BWDirectionalLight* DirctionalLight) { }

	virtual void RenderRenderableShadow(BWRenderable * Renderable) { };

	virtual void BeginDeferLight();
	virtual void DirectLightPass();
	virtual void PointLightPass();

	virtual void SetProjectedShadowInfoForRenderShadow(ShadowMapProjectInfo& ProjectInfo){ }
	virtual void RemoveProjectedShadowInfoFromRenderShadow(ShadowMapProjectInfo& ProjectInfo){ }

	
	BWMaterialPtr mSkyBoxM;
	BWGpuProgramUsagePtr  mSkyBoxGpuPrgramUsage;
	BWHighLevelGpuProgramPtr mSkyBoxProgram;
	void RenderSkyBox();

	BWMaterialPtr mProcessEvnMap;
	BWTexturePtr mProcessEvnMapTexture;
	BWGpuProgramUsagePtr  mProcessEvnMapGpuPrgramUsage;
	BWHighLevelGpuProgramPtr mProcessEvnMapProgram;
	BWTexturePtr IBL_Diffuse_Cube_Map;

	BWMaterialPtr mPreprocessEvnMapForSpecular;
	BWGpuProgramUsagePtr  mProcessEvnMapForSpecularGpuPrgramUsage;
	BWHighLevelGpuProgramPtr mProcessEvnMapForSpecularProgram;
	BWTexturePtr IBL_Specular_Cube_Map;

	BWMaterialPtr mPreprocessEvnMapLUT;
	BWGpuProgramUsagePtr  mProcessEvnMapLUTGpuPrgramUsage;
	BWHighLevelGpuProgramPtr mProcessEvnMapLUTProgram;
	BWTexturePtr IBL_LUT;

	BWMaterialPtr mConverEquirectangularToCubeMap;
	BWTexturePtr HDRCubeMap;
    // SH
	BWTexturePtr AccumulationCubeMaps[2];

};


#endif