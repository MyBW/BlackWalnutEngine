#pragma once
#include "BWRenderState.h"
#include "GL/GL.h"
class GLStaticRasterizerState : public RasterizerStateHI
{
	GLenum FillMode;
	GLenum CullMode;
	GLenum IsEbaleLineAA;
	GLenum IsEnableMSAA;
};

class GLStaticDepthAndStencilState :public DepthAndStencilStateHI
{
	GLenum IsEnableDepthTest;
	GLenum IsEnableDepthWrite;
	GLenum DepthTestFun;
};
