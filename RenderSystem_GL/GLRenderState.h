#pragma once
#include "..\BWEngineCore\BWRenderState.h"
#include "GL/GL.h"
class GLStaticRasterizerState : public RasterizerStateHI
{
public:
	GLenum FillMode;
	GLenum CullMode;
	GLenum IsEbaleLineAA;
	GLenum IsEnableMSAA;
};

class GLStaticDepthAndStencilState :public DepthAndStencilStateHI
{
public:
	GLenum IsEnableDepthTest;
	GLenum IsEnableDepthWrite;
	GLenum DepthTestFun;
};

class GLStaticSamplerState : public SamplerStateHI
{
public:
	GLenum Fileter;
	GLenum RAdd_Mode;
	GLenum SAdd_Mode;
	GLenum TAdd_Mode;
	int MipBias;
};
class GLStaticBlendState : public BlendStateHI
{
public:
	GLenum IsEnbaleBlend;
	GLenum BlendEquation;
	GLenum FactorS;
	GLenum FactorD;
};