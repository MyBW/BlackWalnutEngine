#include "BWPass.h"
#include "BWStringConverter.h"
#include "BWGpuProgramUsage.h"
BWPass::BWPass() : parent(NULL)
, ambient(ColourValue::White)
, diffuse(ColourValue::White)
, specular(ColourValue::Black)
, emissive(ColourValue::Black)
, shininess(0)
, vertexColorType(TVC_NONE)
, src(SBF_ONE)
, des(SBF_ZERO)
, srcAlpha(SBF_ONE)
, desAlpha(SBF_ZERO)
, mSeprateBlend(false)
, sceneBlendOp(SBO_ADD)
, alphaOp(SBO_ADD)
, seprateBlendOption(false)
, depthCheckEnable(true)
, depthWriteEnable(true)
, depthCmpFun(CMPF_LESS_EQUAL)
, constant_bias(0.0f)
, slopscale_bias(0.0f)
, depthBiasPerIteration(0.0f)
, colorwritedEnable(true)
, alphaRejictionFun(CMPF_ALWAYS_PASS)
, alphaRejiction(0)
, alphaToCoverEnable(false)
, transparentSortingEnabled(true)
, transparentSortingForced(false)
, cullingMode(CULL_CLOCKWISE)
, manualCullingMode(MANUAL_CULL_BACK)
, lightEnable(true)
, maxSimulationLight(OGRE_MAX_SIMULTANEOUS_LIGHTS)
, startLight(0)
, iteratePerLight(false)
, lightCountPerItor(1)
, onlyForOneLight(false)
, lightType(BWLight::LT_POINT)
, lightMask(0xFFFFFFFF)
, shadingMode(SO_GOURAUD)
, mPolygonMode(PM_SOLID)
, normalizeEnable(false)
, polygonModeOverrideable(true)
, fogOverride(false)
, fogMode(FOG_NONE)
, fogColor(ColourValue::White)
, start(0.0)
, end(1.0)
, density(0.001)
, srcColor(SBF_ONE)
, desColor(SBF_ZERO)
, mGPUProgramUsage(NULL)
//, mVertexProgramUsage(0)
//, mShadowCasterVertexProgramUsage(0)
//, mShadowCasterFragmentProgramUsage(0)
//, mShadowReceiverVertexProgramUsage(0)
//, mFragmentProgramUsage(0)
//, mShadowReceiverFragmentProgramUsage(0)
//, mGeometryProgramUsage(0)
//, mQueuedForDeletion(false)
, passItorCount(1)
, pointSize(1.0f)
, pointMinSize(0.0f)
, pointMaxSize(0.0f)
, spritePointEnable(false)
, spriteChangeEnable(false)
, contentTypeLookupBuilt(false)
, lightScissoringEnabled(false)
, lightClipPlanesEnabled(false)
, illuminationStage(IS_UNKNOW)
, mVertexPrg(NULL)
, mFragmentPrg(NULL)
{
	
}
BWPass::~BWPass()
{

}
void BWPass::SetParent(BWTechnique *parent)
{
	this->parent = parent;
}
void BWPass::SetName(const std::string &name)
{
	this->name = name;
}
void BWPass::setIndex(size_t index)
{
	mIndex = index;
}
size_t BWPass::getIndex() const
{
	return mIndex;
}
void BWPass::SetVertexColorTracking(TrackVertexColourType type)
{
	vertexColorType = type;
}

void BWPass::SetAmbient(const ColourValue& color)
{
	ambient = color;
}
void BWPass::SetDiffuse(const ColourValue& color)
{
	diffuse = color;
}

void BWPass::SetSpecular(const ColourValue& color)
{
	specular = color;
}
void BWPass::SetSelfIllumination(const ColourValue& color)
{
	emissive = color;
}
void BWPass::SetShininess(float shininess)
{
	this->shininess = shininess;
}

void BWPass::SetSceneBlendType(SceneBlendType type)
{
	sceneBlendType = type;
}

void BWPass::SetSceneBlending(SceneBlendFactor src, SceneBlendFactor des)
{
	this->src = src;
	this->des = des;
}

void BWPass::SetSeparateSceneBlending(SceneBlendType color, SceneBlendType alpha)
{
	colorBlendType = color;
	alphaBlendType = alpha;
}

void BWPass::SetSeparateSceneBlending(SceneBlendFactor srcColor, SceneBlendFactor desColor, SceneBlendFactor srcAlpha, SceneBlendFactor desAlpha)
{
	this->srcColor = srcColor;
	this->desColor = desColor;
	this->srcAlpha = srcAlpha;
	this->desAlpha = desAlpha;
}

void BWPass::SetSceneBlendingOperation(SceneBlendOperation op)
{
	sceneBlendOp = op;
}

void BWPass::SetSeparateSceneBlendingOperation(SceneBlendOperation colorOp, SceneBlendOperation alphaOp)
{
	this->colorOp = colorOp;
	this->alphaOp = alphaOp;
}

void BWPass::SetDepthCheckEnabled(bool enable)
{
	depthCheckEnable = enable;
}

void BWPass::SetDepthWriteEnabled(bool enable)
{
	depthWriteEnable = enable;
}

void BWPass::SetDepthBias(float bias1, float bias2)
{
	constant_bias = bias1;
	slopscale_bias = bias2;
}

void BWPass::SetDepthFunction(CompareFunction cmpFun)
{
	depthCmpFun = cmpFun;
}

void BWPass::SetAlphaRejectFunction(CompareFunction cmpFun)
{
	alphaRejictionFun = cmpFun;
}

void BWPass::SetAlphaRejectSettings(CompareFunction cmpFun, unsigned char num)
{
	alphaRejictionFun = cmpFun;
	alphaRejiction = num;
}
void BWPass::SetAlphaToCoverageEnabled(bool cover)
{
	alphaToCoverEnable = cover;
}
void BWPass::SetLightClipPlanesEnabled(bool enable)
{
	lightClipPlanesEnabled = enable;
}
void BWPass::SetLightScissoringEnabled(bool enable)
{
	lightScissoringEnabled = enable;
}

void BWPass::SetTransparentSortingEnabled(bool enable)
{
	transparentSortingEnabled = enable;
}

void BWPass::SetTransparentSortingForced(bool enable)
{
	transparentSortingForced = enable;
}

void BWPass::SetIlluminationStage(IlluminationStage stage)
{
	illuminationStage = stage;
}
void BWPass::SetCullingMode(CullingMode mode)
{
	cullingMode = mode;
}
void BWPass::SetManualCullingMode(ManualCullingMode mode)
{
	manualCullingMode = mode;
}

void BWPass::SetNormaliseNormals(bool enable)
{
	normalizeEnable = enable;
}

void BWPass::SetLightEnabled(bool enable)
{
	lightEnable = enable;
}

void BWPass::SetShadingMode(ShadeOptions shadingMode)
{
	this->shadingMode = shadingMode;
}

void BWPass::SetPolygonMode(PolygonMode mode)
{
	mPolygonMode = mode;
}
void BWPass::SetPolygonModeOverrideable(bool enable)
{
	polygonModeOverrideable = enable;
}
void BWPass::SetFog(bool enable, FogMode mode, ColourValue &color, float density, float start, float end)
{
	this->fogEnable = enable;
	this->fogMode = mode;
	this->density = density;
	this->start = start;
	this->end = end;
}

void BWPass::SetColorWriteEnable(bool enable)
{
	colorwritedEnable = enable;
}

void BWPass::SetMaxSimulationLight(unsigned int num)
{
	maxSimulationLight = num;
}

void BWPass::SetStartLight(unsigned int num)
{
	startLight = num;
}

void BWPass::SetLightMask(unsigned int num)
{
	lightMask = num;
}

void BWPass::SetLightCountPerIteration(unsigned short num)
{
	lightCountPerItor = num;
}

void BWPass::SetPassIterationCount(int count)
{
	passItorCount = count;
}

void BWPass::SetIteratePerLight(bool enable , bool onlyForOneLightType /* = true */, BWLight::LightType type /* = BWLight::LT_POINT */)
{
	iteratePerLight = enable;
	onlyForOneLight = onlyForOneLightType;
	lightType = type;
}

void BWPass::SetPointSize(float size)
{
	pointSize = size;
}

void BWPass::SetPointMaxSize(float max)
{
	pointMaxSize = max;
}
void BWPass::SetPointMinSize(float min)
{
	pointMinSize = min;
}

void BWPass::SetPointSpriteEnable(bool enable)
{
	spritePointEnable = enable;
}
void BWPass::SetPointAttenuation(bool enable , float constant /* = 0.0 */, float linear /* = 1.0 */, float quadratic /* = 0.0 */)
{
	this->spriteChangeEnable = enable;
	this->constant = constant;
	this->linear = linear;
	this->quadratic = quadratic;
}

int BWPass::GetVertexColourTracking()
{
	return vertexColorType;
}
void BWPass::SetIterationDepthBias(float bias)
{
	depthBiasPerIteration = bias;
}

void BWPass::SetVertexProgram(const std::string &name)
{
	if (!mVertexPrg)
	{
		mVertexPrg = new BWGpuProgramUsage(this);
	}
	mVertexPrg->SetGpuProgram(name);
}
BWGpuProgramPtr BWPass::GetVertexProgram()  const 
{
	if (!mVertexPrg)
	{
		return NULL;
	}
	return mVertexPrg->GetGpuProgram();
}
bool BWPass::hasVertexProgram() const 
{
	return mVertexPrg != NULL;
}
BWGpuProgramParametersPtr BWPass::GetVertexProgramParameter()
{
	if (!mVertexPrg)
	{
		return NULL;
	}
	return mVertexPrg->GetGpuProgramParameter();
}
bool BWPass::hasFragmentProgram() const 
{
	 return mFragmentPrg != NULL;
}
void BWPass::SetFragmentProgram(const std::string &name)
{
	if (!mFragmentPrg)
	{
		mFragmentPrg = new BWGpuProgramUsage(this);
	}
	mFragmentPrg->SetGpuProgram(name);
}
BWGpuProgramPtr BWPass::GetFragmetProgram() const
{
	if (!mFragmentPrg)
	{
		return NULL;
	}
	return mFragmentPrg->GetGpuProgram();
}

BWGpuProgramParametersPtr BWPass::GetFragmetProgramParameter()
{
	if (!mFragmentPrg)
	{
		return NULL;
	}
	return mFragmentPrg->GetGpuProgramParameter();
}
BWTextureUnitState * BWPass::CreateTextureUnitState()
{
	BWTextureUnitState * t = new BWTextureUnitState(this);
	AddTextureUnitState(t);
	return t;
}

void BWPass::AddTextureUnitState(BWTextureUnitState *textureUnitState)
{
	if (textureUnitState)
	{
		if (textureUnitState->getParent() == 0 || textureUnitState->getParent() == this)
		{
			mTextureUnitStates.push_back(textureUnitState);
			if (textureUnitState->getName().empty())
			{
				size_t idx = mTextureUnitStates.size() - 1;
				textureUnitState->setName(StringConverter::ToString((int)idx));
				textureUnitState->setTextureNameAlias(StringUtil::BLANK);
				textureUnitState->SetIndex(idx);
			}
		}
		else
		{
			//异常
		}
	}
}
bool BWPass::IsLoaded()
{
	return parent->isLoaded();
}

void BWPass::load()
{
	TextureUnitStates::iterator itor = mTextureUnitStates.begin();
	while (itor != mTextureUnitStates.end())
	{
		(*itor)->_load();
		itor++;
	}
	// 然后载入GPU程序
}

BWTextureUnitState* BWPass::getTextureUnitState(unsigned short index) const
{
	if ( index >= mTextureUnitStates.size())
	{
		return NULL;
	}
	return mTextureUnitStates[index];
}

const ColourValue& BWPass::getAmbient() const
{
	return ColourValue::Black;
}

const ColourValue& BWPass::getDiffuse() const
{
	return ColourValue::Black;
}

const ColourValue& BWPass::getSelfIllumination() const
{
	return ColourValue::Blue;
}

const ColourValue& BWPass::getSpecular() const
{
	return ColourValue::Black;
}

unsigned int BWPass::getNumTextureUnitStates() const
{
	return mTextureUnitStates.size();
}
float BWPass::getShininess() const
{
	return 1;
}
bool BWPass::hasSeparateSceneBlending() const 
{
	return mSeprateBlend;
}
SceneBlendFactor  BWPass::getSourceBlendFactor() const
{
	return srcColor;
}
SceneBlendFactor  BWPass::getDestBlendFactor() const
{
	return desColor;
}
SceneBlendFactor  BWPass::getSourceBlendFactorAlpha() const
{
	return srcAlpha;
}
SceneBlendFactor BWPass::getDestBlendFactorAlpha() const
{
	return desAlpha;
}
bool  BWPass::hasSeparateSceneBlendingOperation() const
{
	return seprateBlendOption;
}
SceneBlendOperation  BWPass::getSceneBlendingOperation() const
{
	return sceneBlendOp;
}
SceneBlendOperation  BWPass::getSceneBlendingOperationAlpha() const
{
	return alphaOp;
}


float BWPass::getPointSize() const
{
	return  pointSize;
}
bool  BWPass::isPointAttenuationEnabled() const
{
	return spriteChangeEnable;
}
float BWPass::getPointAttenuationConstant() const
{
	return constant;
}
float BWPass::getPointAttenuationLinear() const
{
	return linear;
}
float BWPass::getPointAttenuationQuadratic() const
{
	return quadratic;
}
float BWPass::getPointMinSize() const
{
	return pointMinSize;
}
float BWPass::getPointMaxSize() const
{
	return pointMaxSize;
}
PolygonMode BWPass::getPolygonMode() const
{
	return mPolygonMode;
}
ShadeOptions BWPass::getShadingMode() const
{
	return shadingMode;
}

CullingMode BWPass::getCullingMode() const
{
	return cullingMode;
}

bool BWPass::getColourWriteEnable() const
{
	return colorwritedEnable;
}

CompareFunction BWPass::getAlphaRejectFuntion() const
{
	return alphaRejictionFun;
}
unsigned char BWPass::getAlphaRejectValue() const
{
	return alphaRejiction;
}

bool BWPass::isAlphaToCoverageEnable() const
{
	return alphaToCoverEnable;
}
CompareFunction BWPass::getDepthFunction() const
{
	return depthCmpFun;
}

bool BWPass::getDepthCheckEnable() const
{
	return depthCheckEnable;
}
bool BWPass::getDepthWriteEnable() const
{
	return depthWriteEnable;
}

float BWPass::getDepthBiasConstant() const
{
	return constant_bias;
}
float BWPass::getDepthBiasSlopScale() const
{
	return slopscale_bias;
}
bool BWPass::isProgrammable() const
{
	return mFragmentPrg || mVertexPrg || mGPUProgramUsage.Get();
}
bool BWPass::getPolygonModeOverrideable() const
{
	return polygonModeOverrideable;
}
bool BWPass::getNormaliseNormals() const
{
	return normalizeEnable;
}

unsigned int BWPass::getStartLight() const
{
	return 0;
}



BWHighLevelGpuProgramPtr BWPass::getGPUProgram() const
{
	if (!mGPUProgramUsage.IsNull())
	{
		return mGPUProgramUsage->GetGpuProgram();
	}
	return NULL;
}
void BWPass::setGPUProgram(const std::string &name , const std::string &language)
{
	if (mGPUProgramUsage.IsNull())
	{
		mGPUProgramUsage = new BWGpuProgramUsage(this);
	}
	mGPUProgramUsage->setGPUProgram(name, language);
}
BWGpuProgramParametersPtr BWPass::getGPUProgramParameter() const
{
	if (!mGPUProgramUsage.IsNull())
	{
		return mGPUProgramUsage->GetGpuProgramParameter();
	}
	return NULL;
}
BWGpuProgramUsagePtr BWPass::getGPUProgramUsage() const
{
	return mGPUProgramUsage;
}
