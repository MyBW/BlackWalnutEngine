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

class SamplerStateHI : public StateHI
{
};
using SamplerStateHIP = SamplerStateHI*;
using SamplerStateHIRef = SmartPointer<SamplerStateHI>;
struct StaticSamplerStateInitializer 
{
	FilterOptions Filter;
	SamplerAddressMode RAdd_Mode;
	SamplerAddressMode SAdd_Mode;
	SamplerAddressMode TAdd_Mode;

	int MipBias;
};
template<FilterOptions Filter = FO_POINT,
		SamplerAddressMode RAdd_Mode = SAM_CLAMP,
		SamplerAddressMode SAdd_Mode = SAM_CLAMP,
	    SamplerAddressMode TAdd_Mode = SAM_CLAMP,
	     int MipBias = 0
		>
class TStaticSamplerState : public TStaticStateHI<TStaticSamplerState<Filter, RAdd_Mode, SAdd_Mode, TAdd_Mode>, SamplerStateHIRef, SamplerStateHIP>
{
public:
	static SamplerStateHIRef CreateHI();
};

class BlendStateHI : public StateHI
{

};
using BlendStateHIP = BlendStateHI*;
using BlendStateHIRef = SmartPointer<BlendStateHI>;
struct StaticBlendStateInitializer
{
	bool IsEnableBlend;
	SceneBlendOperation BlendEquation;
	SceneBlendFactor FactorS;
	SceneBlendFactor FactorD;
};
template<bool IsEnableBlend = false,
	SceneBlendOperation BlendEquation = SceneBlendOperation::SBO_ADD,
	SceneBlendFactor FactorS = SceneBlendFactor::SBF_ONE,
    SceneBlendFactor FactorD = SceneBlendFactor::SBF_ZERO
	>
class TStaticBlendStateHI : public TStaticStateHI<TStaticBlendStateHI<IsEnableBlend, BlendEquation, FactorS, FactorD>, BlendStateHIRef, BlendStateHIP>
{
public:
	static BlendStateHIRef CreateHI();
};

#include "BWRenderState.inl"