#ifndef GLRENDERSYSTEM_H_
#define GLRENDERSYSTEM_H_
#include "../BWEngineCore/BWRenderSystem.h"
#include "../BWEngineCore/BWSingleton.h"
#include "../BWEngineCore/BWCommon.h"
#include "../BWEngineCore/BWRenderWindow.h"
#include"GLTexture.h"
#include "GLWin32Support.h"
#include "GLSLGpuProgram.h"
class GLContext;
class GLRenderSystem : public BWSingleton<GLRenderSystem> , public BWRenderSystem
{
	
public:
	GLRenderSystem()
	{
		mGLSupport = new GLWin32Support();
		mWorldMatrix = BWMatrix4::IDENTITY;
		mViewMatrix = BWMatrix4::IDENTITY;
		
		initConfigOptions();
		mColourWrite[0] = mColourWrite[1] = mColourWrite[2] = mColourWrite[3] = true;
		for (int i = 0; i < OGRE_MAX_TEXTURE_LAYERS; i++)
		{
			mTextureCoordIndex[i] = 99;
			mTextureType[i] = 0;
		}
		mActiveRenderTarget = NULL;
		mCurrentContext = NULL;
		mMainContext = NULL;
		mGLInitialised = false;
		mActiveTextureUnit = -1;
		mMipFilterOperation = FO_NONE;
	}
	~GLRenderSystem()
	{
	
	}
	static GLRenderSystem * GetInstance();
	bool activateGLTextureUnit(size_t texUnit);
	void clearFrameBuffer(unsigned int buffers, const ColourValue &color /* = ColourValue::Black */, float depth /* = 1.0 */, unsigned short stencil /* = 0 */);
	void setNormaliseNormals(bool enbale) const;
	void setShadingType(ShadeOptions type);
	GLint convertCompareFun(CompareFunction function)  const;
	GLint getBlendMode(SceneBlendFactor factor);
	GLint getTextureAddressingMode(BWTextureUnitState::TextureAddressingMode model);
	void makeGLMatrix(GLfloat gl_matrix[16], const BWMatrix4& m);
	virtual void initialiseFromRenderSystemCapabilities();
	void initConfigOptions();
	void _setViewport(BWViewport *viewport);
	BWRenderWindow* _createRenderWindow(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList* miscParam);
	void unbindGpuProgram(GpuProgramType GPT);
	GLint getCombineMinMipFilter() const ;
	void _setPolygonMode(PolygonMode polygonMode);
	virtual BWRenderTarget* createRenderTarget(const std::string &name);
	virtual void DestroyRenderTarget(const std::string &name);
	virtual void bindGpuProgram(BWGpuProgram *GpuProgamr);
	virtual void bindGPUProgramUsage(BWGpuProgramUsage*gpuPrgramUsage);
	static void GLtranspose(GpuConstantType type, void *data);

	bool InitRendererResource() override;

	//////////////////////////////New Interface
public:
	void SetViewport(int ViewportX, int ViewportY, int ViewportWidth, int ViewportHight, 
		int ScissorX, int ScissorY, int ScissorWidth, int ScissorHigh) override;
	void SetRenderTarget(RSRenderTarget& InRenderTarget) override;
	void SetGrphicsPipelineState(RSGraphicPipelineState& InPipelineState) override;
	void SetShaderTexture(BWHighLevelGpuProgramPtr GPUProgram, BWTexturePtr Texture, SamplerStateHIRef Sampler) override;
	void ClearRenderTarget(unsigned int buffers, const ColourValue &color  = ColourValue::Black , float depth  = 1.0 , unsigned short stencil = 0 );
	void ReadSurfaceData(BWTexturePtr SourceTexture, int Index, int MipLevel, BWPixelBox& Destination) override;
	RasterizerStateHIRef CreateRasterizerStateHI(RasterStateInitializer& Initializer) override;
	DepthAndStencilStateHIRef CreateDepthAndStencilHI(DepthAndStencilInitializer& Initializer) override;
	SamplerStateHIRef CreateSamplerStateHI(StaticSamplerStateInitializer& Initializer) override;
public:
	//////////////////////////////New Interface End


protected:
	BWRenderWindow* _initialise(bool autoCreateWindow, const std::string &windowTitle);
	void _oneTimeContextInitialization();
	void _setTexture(size_t texUnit, bool enable, const BWTexturePtr &textPtr);
	void _setTextureUnitFiltering(size_t texUnit, FilterType type , FilterOptions op);
	void _setTexturelayerAnisotropy(size_t texUnit, unsigned int anisotropy);
	void _setCullingMode(CullingMode cullingMode);
	void _convertProjectionMatrix(const BWMatrix4& matrix, BWMatrix4 &dest, bool forGpuProgram /* = false */);
	void _setColourBufferEnable(bool r, bool g, bool b, bool a);
	void _setAlphaRejectSetting(CompareFunction rejectFunction, unsigned char rejectValue, bool alphToCoverage);
	void _setDepthBias(float deptBias, float deptBiaseSlopScale);
	void _setDepthBufferWriteEnable(bool enbale);
	void _setDepthBufferCheckEnable(bool enable);
	void _setDepthBufferFunction(CompareFunction depthFunction);
	void _setSeparatesceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceAlphaFactor, SceneBlendFactor destAlphaFactor, SceneBlendOperation op /* = SBO_ADD */, SceneBlendOperation alphaOp /* = SBO_ADD */);
	void _setSceneBlending(SceneBlendFactor sourceFactro, SceneBlendFactor destFactor, SceneBlendOperation op);
	void _setPointParameters(float size, bool attenuationEnable, float constant, float linear, float quadratic, float minSize, float maxSize);
	void _setPointSpriteEnable(bool enable);
	void _setTextureMipmapBias(size_t texNum, float bias);
	void _setTextureBlendMode(size_t texNum, const LayerBlendModeEx &model);
	void _setTextureBorderColour(size_t texNum, const ColourValue &color);
	void _setTextureAddressionMode(size_t texNum, const BWTextureUnitState::UVWAddressingMode &uvw);
	void _setTextureCoordSet(size_t texUnit, unsigned int textureCoordSet);
	void _setTextureCoordCalculation(size_t texNum, TexCoordCalcMethod texCoordCalcMethod, const BWFrustum * frustum /* = NULL */);
	void _setTextureMatrix(size_t texNum, const BWMatrix4 &matrix);

	void _setWorldMatrices(const BWMatrix4* m, unsigned short count);
	void _setWorldMatrix(const BWMatrix4 &m);
	void _setViewMatrix(const BWMatrix4 &m);
	void _setProjectionMatrix(const BWMatrix4 & matrix);
	void _switcheContext(GLContext *context);
	void _setRenderTarget(BWRenderTarget *renderTarget);
	void _render(const BWRenderOperation &ro);
	void initialiseContext(BWRenderWindow *primary);

	void SetRenderShadowMapRenderTarget() override;
	void SetLightPreparForRenderShadow(BWDirectionalLight * DirectionalLight) override;
	void RemoveLightFromRenderShadow(BWDirectionalLight* DirctionalLight);
	void RenderRenderableShadow(BWRenderable * Renderable) override;
	void FinishLightsShadowMaps() override;
	void BeginDeferLight() override;
	void DirectLightPass() override;
	void PointLightPass() override;
	void RenderSkyBox() override;
	void RenderAmbientOcclusion() override;

	void SetProjectedShadowInfoForRenderShadow(ShadowMapProjectInfo& ProjectInfo) override;
	void RemoveProjectedShadowInfoFromRenderShadow(ShadowMapProjectInfo& ProjectInfo)override;


	void RenderRenderOperation(BWRenderOperation & RenderOperation , BWMatrix4& ModelMatrix);
	void RenderOperation(BWRenderOperation & RenderOperation, GLSLGpuProgram * GPUProgram);
	void RenderRenderOperationWithPointLight(BWRenderOperation &RenderOperation);
	void RenderRenderOperationWithSkyBox(BWRenderOperation &RenderOperation);
private:
	GLenum mTextureType[OGRE_MAX_TEXTURE_LAYERS];
	size_t mTextureCoordIndex[OGRE_MAX_TEXTURE_LAYERS];
	int MaxActiveTexteureNum{ 8 };
	GLSupport *mGLSupport;
	GLContext *mMainContext;
	GLContext *mCurrentContext;
	bool mGLInitialised;
	unsigned short mFixeFunctionTexutreUnits;
	unsigned short mActiveTextureUnit;
	bool mColourWrite[4];
	bool mDepthWrite;
	bool mStencilMask;
	bool mNormaliseNormal;
	bool mUseAutoTextureMatrix;
	GLfloat mAutoTextureMatrix[16];
	BWMatrix4 mViewMatrix;
	BWMatrix4 mWorldMatrix;

	FilterOptions mMinFilterOperation;
	FilterOptions mMipFilterOperation;
	BWGpuProgramPtr mGPUProgram;



	class GLRenderTarget* GLGBuffer;
	class GLHardwarePixelBuffer* FinalRenderResult;


	class GLSLGpuProgram* DirectLightGLSLProgram;
	class BWGpuProgramUsage*  DirectLightGLSLPrgramUsage;


	GLint DirectLightShadowMapLoc;


	class GLSLGpuProgram* mPointLightDefferLightingGLSLProgram;
	class BWGpuProgramUsage*  mPointLightDefferLightingGpuPrgramUsage;


	/*GLint mPointLightDefferLightingBaseColorMapLoc;
	GLint mPointLightDefferLightingNormalMapLoc;
	GLint mPointLightDefferLightingPositionMapLoc;*/

	GLTexturePtr GLBaseColorTexture;
    GLTexturePtr GLNormalTexture;
	GLTexturePtr GLPositionTexture;

	BWRenderOperation CubeMeshRenderOperation;
	BWRenderOperation SphereMeshRenderOperation;

	class GLRenderTarget* GLShadowMapRenderTarget;

	class GLSLGpuProgram* PointLightShadowMapGLSLProgram;
	class BWGpuProgramUsage*  PointLightShadowMapGpuPrgramUsage;

	class GLSLGpuProgram* DirectionalLightShadowMapGLSLProgram;
	class BWGpuProgramUsage*  DirectionalLightShadowMapGpuPrgramUsage;
	

	class GLSLGpuProgram* mSkyBoxGLSLProgram;
	class BWGpuProgramUsage*  mSkyBoxGpuPrgramUsage;
	GLTexturePtr GLSkyBoxTexture;

	//class BWGpuProgramUsage*  AmbientOcclusionGpuPrgramUsage;
	//class GLSLGpuProgram*  AmbientOcclusionProgram;
	//class BWGpuProgramUsage*  AmbientOcclusionFilterGpuPrgramUsage;
	//class GLSLGpuProgram*  AmbientOcclusionFilterProgram;
};


#endif