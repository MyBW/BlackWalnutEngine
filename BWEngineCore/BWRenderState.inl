#include "BWRenderState.h"
template<PolygonMode FillMode /*= PM_SOLID*/, CullingMode CullMode /*= CULL_NONE*/, bool IsEnableLineAA /*= false*/, bool IsEnableMSAA /*= false*/>
RasterizerStateHIRef TStaticRasterizerState<FillMode, CullMode, IsEnableLineAA, IsEnableMSAA>::CreateHI()
{
	RasterStateInitializer Initializer = { FillMode , CullMode , IsEnableLineAA , IsEnableMSAA };
	return BWRoot::GetInstance()->mActiveRenderSystem->CreateRasterizerStateHI(Initializer);
}

template<bool IsEnableDepthTest /*= true*/,
	bool IsEnableDepthWrite /*= true*/,
	CompareFunction DepthTestFun /*= CMPF_LESS_EQUAL
								 */>
	DepthAndStencilStateHIRef TStaticDepthAndStencilState<IsEnableDepthTest, IsEnableDepthWrite, DepthTestFun>::CreateHI()
{
	DepthAndStencilInitializer Initializer = { IsEnableDepthTest , IsEnableDepthWrite , DepthTestFun };
	return BWRoot::GetInstance()->mActiveRenderSystem->CreateDepthAndStencilHI(Initializer);
}


template<FilterOptions Filter /*= FO_POINT*/,
	SamplerAddressMode RAdd_Mode /*= SAM_CLAMP*/,
	SamplerAddressMode SAdd_Mode /*= SAM_CLAMP*/,
	SamplerAddressMode TAdd_Mode /*= SAM_CLAMP*/,
	int MipBias /*= 0
				*/>
SamplerStateHIRef TStaticSamplerState<Filter, RAdd_Mode, SAdd_Mode, TAdd_Mode, MipBias>::CreateHI()
{
	StaticSamplerStateInitializer Initializer = { Filter, RAdd_Mode, SAdd_Mode, TAdd_Mode, MipBias };
	return BWRoot::GetInstance()->mActiveRenderSystem->CreateSamplerStateHI(Initializer);
}


template < bool IsEnableBlend /*= false*/,
	SceneBlendOperation RGBBlendEquation /*= SceneBlendOperation::SBO_ADD*/,
	SceneBlendFactor RGBFactorS /*= SceneBlendFactor::SBF_ONE*/,
	SceneBlendFactor RGBFactorD /*= SceneBlendFactor::SBF_ZERO*/,
	bool IsSepatate /*= false*/,
	SceneBlendOperation AlphaBlendEquation /*= SceneBlendOperation::SBO_ADD*/,
	SceneBlendFactor AlphaFactorS /*= SceneBlendFactor::SBF_ONE*/,
	SceneBlendFactor AlphaFactorD /*= SceneBlendFactor::SBF_ZERO
								  */>
	BlendStateHIRef TStaticBlendStateHI<IsEnableBlend, RGBBlendEquation, RGBFactorS, RGBFactorD, IsSepatate, AlphaBlendEquation, AlphaFactorS, AlphaFactorD>::CreateHI()
{
	StaticBlendStateInitializer Initializer = { IsEnableBlend, RGBBlendEquation, RGBFactorS, RGBFactorD, IsSepatate, AlphaBlendEquation, AlphaFactorS, AlphaFactorD };
	return BWRoot::GetInstance()->mActiveRenderSystem->CreateSamplerStateHI(Initializer);
}

