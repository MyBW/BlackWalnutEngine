#ifndef PASS_H_
#define PASS_H_
#include <string>
#include "BWColorValue.h"
#include "OgreBlendMode.h"
#include "BWLight.h"
#include "BWGpuProgram.h"
#include "BWGpuProgramParams.h"
//#include "BWGpuProgramUsage.h"
#include "BWHighLevelGpuProgram.h"
#include "BWTextureUnitState.h"
#include "BWCommon.h"
#include "BWTechnique.h"
#include "BWGpuProgramUsage.h"
enum IlluminationStage
{
	IS_AMBIENT,
	IS_PER_LIGHT,
	IS_DECAL,
	IS_UNKNOW
};
class BWPass
{
	friend class BWTechnique;
public:
	typedef std::vector<BWTextureUnitState*> TextureUnitStates;
	BWPass();
	~BWPass();
	void SetName(const std::string &name);
	void setIndex(size_t index);
	size_t getIndex() const;
	int GetVertexColourTracking();
	void SetVertexColorTracking(TrackVertexColourType);
	void SetAmbient(const ColourValue&);
	void SetDiffuse(const ColourValue&);
	void SetShininess(float);
	void SetSpecular(const ColourValue&);
	void SetSelfIllumination(const ColourValue&);
	void SetSceneBlendType(SceneBlendType blendType);
	void SetSceneBlending(SceneBlendFactor, SceneBlendFactor);
	void SetSeparateSceneBlending(SceneBlendType, SceneBlendType);
	void SetSeparateSceneBlending(SceneBlendFactor, SceneBlendFactor, SceneBlendFactor, SceneBlendFactor);
	void SetSceneBlendingOperation(SceneBlendOperation);
	void SetSeparateSceneBlendingOperation(SceneBlendOperation, SceneBlendOperation);
	void SetDepthCheckEnabled(bool);
	void SetDepthWriteEnabled(bool);
	void SetDepthBias(float, float);
	void SetDepthFunction(CompareFunction);
	void SetIterationDepthBias(float);
	void SetAlphaRejectSettings(CompareFunction, unsigned char);
	void SetAlphaRejectFunction(CompareFunction);
	void SetAlphaToCoverageEnabled(bool);
	void SetLightScissoringEnabled(bool);
	void SetLightClipPlanesEnabled(bool);
	void SetTransparentSortingEnabled(bool);
	void SetTransparentSortingForced(bool);
	void SetIlluminationStage(IlluminationStage);
	void SetCullingMode(CullingMode);
	//这种裁剪方式是基于顶点的法线 再将数据传输到API的时候 先用引擎裁剪 通常是有场景管理器完成 一般是作用于静态物体 对动态物体十分耗时
	void SetManualCullingMode(ManualCullingMode);
	void SetNormaliseNormals(bool);
	void SetLightEnabled(bool);
	void SetShadingMode(ShadeOptions);
	void SetPolygonMode(PolygonMode);
	void SetPolygonModeOverrideable(bool);
	void SetFog(bool, FogMode, ColourValue&, float, float, float);
	void SetColorWriteEnable(bool);
	void SetMaxSimulationLight(unsigned int);
	void SetStartLight(unsigned int);
	void SetLightMask(unsigned int);
	void SetLightCountPerIteration(unsigned short);
	void SetPassIterationCount(int);
	void SetIteratePerLight(bool, bool onlyForOneLightType = true, BWLight::LightType type = BWLight::LT_POINT);
	void SetPointSize(float);
	void SetPointSpriteEnable(bool);
	void SetPointMinSize(float);
	void SetPointMaxSize(float);
	//使用顶点模式 或者 pointsprite模式时 设置点的变换方式  例如 随着距离的改变而改变
	void SetPointAttenuation(bool, float constant = 0.0, float linear = 1.0, float quadratic = 0.0);

	void SetFragmentProgram(const std::string &name);
	BWGpuProgramPtr GetFragmetProgram() const;
	BWGpuProgramParametersPtr GetFragmetProgramParameter();
	bool hasFragmentProgram() const;

	void SetVertexProgram(const std::string &name);
	BWGpuProgramPtr GetVertexProgram() const ;
	BWGpuProgramParametersPtr GetVertexProgramParameter();
	bool hasVertexProgram() const ;

	size_t getNumTextureUnitStates() const ;
	BWTextureUnitState * getTextureUnitState(unsigned short index) const ;
	const ColourValue & getAmbient() const;
	const ColourValue &getDiffuse() const;
	const ColourValue &getSpecular() const;
	const ColourValue &getSelfIllumination() const;
	float getShininess() const;

	bool hasSeparateSceneBlending() const;
	SceneBlendFactor getSourceBlendFactor() const;
	SceneBlendFactor getDestBlendFactor() const;
	SceneBlendFactor getSourceBlendFactorAlpha() const;
	SceneBlendFactor getDestBlendFactorAlpha() const;
	bool hasSeparateSceneBlendingOperation() const;
	SceneBlendOperation getSceneBlendingOperation() const;
	SceneBlendOperation getSceneBlendingOperationAlpha() const;

	float getPointSize() const;
	bool  isPointAttenuationEnabled() const;
	float getPointAttenuationConstant() const;
    float getPointAttenuationLinear() const;
    float getPointAttenuationQuadratic() const;
    float getPointMinSize() const;
    float getPointMaxSize() const;

	size_t getStartLight() const;
	PolygonMode getPolygonMode() const;
	ShadeOptions getShadingMode() const;
	CullingMode  getCullingMode() const;
	bool getColourWriteEnable() const;
	CompareFunction getAlphaRejectFuntion() const;
	unsigned char getAlphaRejectValue() const;
	bool isAlphaToCoverageEnable() const;
	CompareFunction getDepthFunction() const;
	bool getDepthCheckEnable() const;
	bool getDepthWriteEnable() const;
	float getDepthBiasConstant() const;
	float getDepthBiasSlopScale() const;
	bool getNormaliseNormals() const;
	bool isProgrammable() const;
	virtual bool getPolygonModeOverrideable() const;
	BWTextureUnitState* CreateTextureUnitState();
	void AddTextureUnitState(BWTextureUnitState *textureUnitState);
	BWTechnique* GetParent() const { return parent; }
	void SetParent(BWTechnique *parent);
	std::string GetGroupName() { return parent->GetGroupName(); }
	bool IsLoaded();

	void setGPUProgram(const std::string &name ,  const std::string &language);
	BWHighLevelGpuProgramPtr getGPUProgram() const;
	BWGpuProgramParametersPtr getGPUProgramParameter() const;
	BWGpuProgramUsagePtr getGPUProgramUsage() const ;
protected:
	void load();
private:
	

	std::string name;
	size_t mIndex;
	BWTechnique* parent;
	TrackVertexColourType vertexColorType;
	ColourValue ambient;
	ColourValue diffuse;
	ColourValue specular;
	float shininess;
	ColourValue emissive;

	SceneBlendType sceneBlendType;
	SceneBlendFactor src;
	SceneBlendFactor des;
	SceneBlendType  colorBlendType;
	SceneBlendType  alphaBlendType;

	SceneBlendFactor srcColor;
	SceneBlendFactor desColor;
	SceneBlendFactor srcAlpha;
	SceneBlendFactor desAlpha;

	SceneBlendOperation sceneBlendOp;
	SceneBlendOperation alphaOp;

	SceneBlendOperation colorOp;

	bool mSeprateBlend; // 是否设置与场景混合参数
	bool seprateBlendOption; //  颜色混合是否和alpha混合分开

	bool depthCheckEnable;
	bool depthWriteEnable;
	float constant_bias;
	float slopscale_bias;
	float depthBiasPerIteration;
	CompareFunction depthCmpFun;

	bool colorwritedEnable;
	CompareFunction alphaRejictionFun;
	unsigned char alphaRejiction;

	bool alphaToCoverEnable;
	
	bool transparentSortingEnabled;
	bool transparentSortingForced;

	CullingMode cullingMode;
	ManualCullingMode manualCullingMode;

	bool  normalizeEnable;
	ShadeOptions shadingMode;
	PolygonMode mPolygonMode;
	bool polygonModeOverrideable;

	bool fogEnable;
	FogMode fogMode;
	ColourValue fogColor;
	bool fogOverride;
	float density;
	float start;
	float end;

	bool  lightEnable;
	unsigned int maxSimulationLight;
	unsigned int startLight;
	unsigned int lightMask;
	unsigned short lightCountPerItor;
	bool iteratePerLight;
	bool onlyForOneLight;
	BWLight::LightType lightType;
	bool lightClipPlanesEnabled;
	bool lightScissoringEnabled;
	IlluminationStage illuminationStage;

	int passItorCount;


	float pointSize;
	float pointMaxSize;
	float pointMinSize;
	bool  spritePointEnable;
	bool spriteChangeEnable;
	float constant;
	float linear;
	float quadratic;
	bool  contentTypeLookupBuilt;

	BWGpuProgramUsage *mVertexPrg;
	BWGpuProgramUsage *mFragmentPrg;
	TextureUnitStates mTextureUnitStates;

	BWHighLevelGpuProgramPtr mHeightLevelProgram;
	BWGpuProgramUsagePtr mGPUProgramUsage;
};


#endif