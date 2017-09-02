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
	SamplerAddressMode UAdd_Mode /*= SAM_CLAMP*/,
	SamplerAddressMode VAdd_Mode /*= SAM_CLAMP*/,
	SamplerAddressMode WAdd_Mode /*= SAM_CLAMP*/,
	int MipBias /*= 0
				*/>
StaticSamplerStateHIRef TStaticSamplerState<Filter, UAdd_Mode, VAdd_Mode, WAdd_Mode, MipBias>::CreateHI()
{
	StaticSamplerStateInitializer Initializer = { Filter, UAdd_Mode, VAdd_Mode, WAdd_Mode, MipBias };
	return BWRoot::GetInstance()->mActiveRenderSystem->CreateSamplerStateHI(Initializer);
}
