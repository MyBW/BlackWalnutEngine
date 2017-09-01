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

// 暂时不包含模板测试
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

