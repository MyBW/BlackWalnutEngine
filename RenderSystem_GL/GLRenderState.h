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
