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
	GLenum MinFilter;
	GLenum MagFilter;
	GLenum RAdd_Mode;
	GLenum SAdd_Mode;
	GLenum TAdd_Mode;
	int MipBias;
};
class GLStaticBlendState : public BlendStateHI
{
public:
	GLenum IsEnableBlend;
	GLenum RGBBlendEquation;
	GLenum RGBFactorS;
	GLenum RGBFactorD;
	GLenum IsSepatate;
	GLenum AlphaBlendEquation;
	GLenum AlphaFactorS;
	GLenum AlphaFactorD;
};

class GLStaticColorMaskState : public ColorMaskStateHI
{
public: 
	GLenum Red;
	GLenum Green;
	GLenum Blue;
	GLenum Alpha;
};