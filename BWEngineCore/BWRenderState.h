#pragma once
#include "BWCommon.h"
#include "BWSmartPointer.h"
template<typename InitializerType, typename HITypeRef, typename HITypeP>
class TStaticStateHI
{
public:
	static HITypeRef GetStateHI()
	{
		static StaticStateResource* StaticState;
		if (!StaticState)
		{
			StaticState = new StaticStateResource();
		}
		return StaticState->State;
	}
private:
	class StaticStateResource
	{
	public:
		HITypeRef State;
		StaticStateResource()
		{
			State = InitializerType::CreateHI();
		}
	};
};

class StateHI
{
public:
	virtual int GetMemSize() { return 0; } 
	bool IsDirty{ false };
};

class RasterizerStateHI : public StateHI
{
};
using RasterizerStateHIP = RasterizerStateHI*;
using RasterizerStateHIRef = SmartPointer<RasterizerStateHI>;
struct RasterStateInitializer
{
	PolygonMode FillMode;
	CullingMode CullMode;
	bool IsEnableLineAA;
	bool IsEnableMSAA;
};
template<PolygonMode FillMode = PM_SOLID, CullingMode CullMode = CULL_NONE, bool IsEnableLineAA = false, bool IsEnableMSAA = false>
class TStaticRasterizerState : public TStaticStateHI<TStaticRasterizerState<FillMode, CullMode, IsEnableLineAA, IsEnableMSAA>, RasterizerStateHIRef, RasterizerStateHIP>
{
public:
	static RasterizerStateHIRef CreateHI();
};

// 暂时不包含模板测试
class DepthAndStencilStateHI : public StateHI
{

};
using DepthAndStencilStateHIP = DepthAndStencilStateHI*;
using DepthAndStencilStateHIRef = SmartPointer<DepthAndStencilStateHI>;
struct DepthAndStencilInitializer
{
	bool IsEnableDepthTest;
	bool IsEnableDepthWrite;
	CompareFunction DepthTestFun;
}; 
template<bool IsEnableDepthTest = true, 
	    bool IsEnableDepthWrite = true, 
		CompareFunction DepthTestFun = CMPF_LESS_EQUAL
	    >
class TStaticDepthAndStencilState :public TStaticStateHI< TStaticDepthAndStencilState<IsEnableDepthTest, IsEnableDepthWrite, DepthTestFun>, DepthAndStencilStateHIRef, DepthAndStencilStateHIP>
{
public:
	static DepthAndStencilStateHIRef CreateHI();
};

class ColorMaskStateHI : public StateHI
{

};
using ColorMaskStateHIP = ColorMaskStateHI*;
using ColorMaskStateHIRef = SmartPointer<ColorMaskStateHI>;
struct StaticColorMaskStateInitializer
{
	bool Red;
	bool Green;
	bool Blue;
	bool Alpha;
};
template<bool Red = true,
	bool Green = true,
	bool Blue = true,
	bool Alpha = true
>
class TStaticColorMaskState :public TStaticStateHI<TStaticColorMaskState<Red, Green, Blue, Alpha>, ColorMaskStateHIRef, ColorMaskStateHIP>
{
public:
	static ColorMaskStateHIRef CreateHI();
};


class SamplerStateHI : public StateHI
{
};
using SamplerStateHIP = SamplerStateHI*;
using SamplerStateHIRef = SmartPointer<SamplerStateHI>;
struct StaticSamplerStateInitializer 
{
	FilterOptions MinFilter;
	FilterOptions MagFilter;
	FilterOptions MipFilter;
	SamplerAddressMode RAdd_Mode;
	SamplerAddressMode SAdd_Mode;
	SamplerAddressMode TAdd_Mode;

	int MipBias;
};
template<
	    FilterOptions MinFilter = FO_POINT,
	    FilterOptions MagFilter = FO_POINT,
	    FilterOptions MipFilter = FO_NONE,
		SamplerAddressMode RAdd_Mode = SAM_CLAMP,
		SamplerAddressMode SAdd_Mode = SAM_CLAMP,
	    SamplerAddressMode TAdd_Mode = SAM_CLAMP,
	     int MipBias = 0
		>
class TStaticSamplerState : public TStaticStateHI<TStaticSamplerState<MinFilter,MagFilter, MipFilter, RAdd_Mode, SAdd_Mode, TAdd_Mode>, SamplerStateHIRef, SamplerStateHIP>
{
public:
	static SamplerStateHIRef CreateHI();
};

template<
	FilterOptions MinFilter /*= FO_POINT*/,
	FilterOptions MagFilter /*= FO_POINT*/,
	FilterOptions MipFilter /*= FO_NONE*/,
	SamplerAddressMode RAdd_Mode /*= SAM_CLAMP*/,
	SamplerAddressMode SAdd_Mode /*= SAM_CLAMP*/,
	SamplerAddressMode TAdd_Mode /*= SAM_CLAMP*/,
	int MipBias /*= 0
   */>
	SamplerStateHIRef TStaticSamplerState<MinFilter, MagFilter, MipFilter, RAdd_Mode, SAdd_Mode, TAdd_Mode, MipBias>::CreateHI()
{
	StaticSamplerStateInitializer Initializer = { MinFilter,MagFilter, MipFilter, RAdd_Mode, SAdd_Mode, TAdd_Mode, MipBias };
	return BWRoot::GetInstance()->mActiveRenderSystem->CreateSamplerStateHI(Initializer);
}

class BlendStateHI : public StateHI
{

};
using BlendStateHIP = BlendStateHI*;
using BlendStateHIRef = SmartPointer<BlendStateHI>;
struct StaticBlendStateInitializer
{
	bool IsEnableBlend;
	SceneBlendOperation RGBBlendEquation;
	SceneBlendFactor RGBFactorS ;
	SceneBlendFactor RGBFactorD ;
	bool IsSepatate;
	SceneBlendOperation AlphaBlendEquation;
	SceneBlendFactor AlphaFactorS ;
	SceneBlendFactor AlphaFactorD ;
};
template < bool IsEnableBlend = false,
	SceneBlendOperation RGBBlendEquation = SceneBlendOperation::SBO_ADD,
	SceneBlendFactor RGBFactorS = SceneBlendFactor::SBF_ONE,
	SceneBlendFactor RGBFactorD = SceneBlendFactor::SBF_ZERO,
	bool IsSepatate = false,
	SceneBlendOperation AlphaBlendEquation = SceneBlendOperation::SBO_ADD,
	SceneBlendFactor AlphaFactorS = SceneBlendFactor::SBF_ONE,
	SceneBlendFactor AlphaFactorD = SceneBlendFactor::SBF_ZERO
	>
class TStaticBlendStateHI : public TStaticStateHI<TStaticBlendStateHI<IsEnableBlend, RGBBlendEquation, RGBFactorS, RGBFactorD, 
	IsSepatate, AlphaBlendEquation, AlphaFactorS , AlphaFactorD>, 
	BlendStateHIRef, 
	BlendStateHIP>
{
public:
	static BlendStateHIRef CreateHI();
};

#include "BWRenderState.inl"