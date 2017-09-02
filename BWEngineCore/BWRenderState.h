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


class RasterizerStateHI
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
	static RasterizerStateHIP CreateHI()
	{
		RasterStateInitializer Initializer = { FillMode , CullMode , IsEnableLineAA , IsEnableMSAA };
		return BWRoot::GetInstance()->mActiveRenderSystem->CreateRasterizerStateHI(Initializer);
	}
};

// ��ʱ������ģ�����
class DepthAndStencilStateHI
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
	    bool IsEnbaleDepthWrite = true, 
		CompareFunction DepthTestFun = CMPF_LESS_EQUAL
	    >
class TStaticDepthAndStencilState :public TStaticStateHI< TStaticDepthAndStencilState<IsEnableDepthTest, IsEnbaleDepthWrite, DepthTestFun>, DepthAndStencilStateHIRef, DepthAndStencilStateHIP>
{
	static DepthAndStencilStateHIP CreateHI()
	{
		DepthAndStencilInitializer Initializer = { IsEnableDepthTest , IsEnableDepthWrite , DepthTestFun };
		return BWRoot::GetInstance()->mActiveRenderSystem->CreateDepthAndStencilHI(Initializer);
	}
};

class StaticSamplerStateHI
{
};
using StaticSamplerStateHIP = StaticSamplerStateHI*;
using StaticSamplerStateHIPtr = SmartPointer<StaticSamplerStateHI>;
struct StaticSamplerStateInitializer 
{
	FilterOptions Filter;
	SamplerAddressMode UAdd_Mode;
	SamplerAddressMode VAdd_Mode;
	SamplerAddressMode WAdd_Mode;
	int MipBias;
};
template<FilterOptions Filter = FO_POINT,
		SamplerAddressMode UAdd_Mode = SAM_CLAMP,
		SamplerAddressMode VAdd_Mode = SAM_CLAMP,
	    SamplerAddressMode WAdd_Mode = SAM_CLAMP,
		int MipBias = 0
		>
class TStaticSamplerState : public TStaticStateHI<TStaticSamplerState<Filter, UAdd_Mode, VAdd_Mode, WAdd_Mode>, StaticSamplerStateHIPtr, StaticSamplerStateHIP>
{
	static StaticSamplerStateP CreateHI()
	{
		StaticSamplerStateInitializer Initializer = {Filter, UAdd_Mode, VAdd_Mode, WAdd_Mode, MipBias};
		return BWRoot::GetInstance()->mActiveRenderSystem->CreateSamplerStateHI(Initializer);
	}
};