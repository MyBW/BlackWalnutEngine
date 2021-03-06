#pragma once
#include "..\BWEngineCore\BWCommon.h"
namespace Helper
{
	FORCEINLINE void SetIsEnableState(GLenum IsEnable, GLenum State)
	{
		if (IsEnable == GL_TRUE)
			glEnable(State);
		else
			glDisable(State);
	}
	FORCEINLINE GLenum GetFillMode(PolygonMode Mode)
	{
		switch (Mode)
		{
		case PM_SOLID:     return GL_FILL;
		case PM_POINTS:    return GL_POINT;
		case PM_WIREFRAME: return GL_LINE;
		}
		Check(0);
	}
	FORCEINLINE GLenum GetBool(bool IsTrue)
	{
		return IsTrue ? GL_TRUE : GL_FALSE;
	}
	GLenum GetCullMode(CullingMode CullMode)
	{
		switch (CullMode)
		{
		case CULL_NONE:          return GL_CULL_FACE;
		case CULL_CLOCKWISE:     return GL_FRONT;
		case CULL_ANTICLOCKWISE: return GL_BACK;
		}
		Check(0);
		return 0;
	}
	GLenum GetGLTextureType(TextureType _TextureType)
	{
		//纹理数组怎么办
		switch (_TextureType)
		{
		case TEX_TYPE_1D:
			return GL_TEXTURE_1D;
		case TEX_TYPE_2D:
			return GL_TEXTURE_2D;
		case TEX_TYPE_3D:
			return GL_TEXTURE_3D;
		case TEX_TYPE_CUBE_MAP:
			return GL_TEXTURE_CUBE_MAP;
		}
		Check(0);
	}
	GLenum GetGLTextureSwap(SamplerAddressMode AddMode)
	{
		switch (AddMode)
		{
		case SAM_WRAP:  return GL_WRAP_BORDER;
		case SAM_CLAMP: return GL_CLAMP_TO_EDGE;
		case SAM_MIRROR: return GL_MIRROR_CLAMP_EXT;
		case SAM_BORDER: return GL_CLAMP_TO_BORDER;
		case SAM_REPEAT: return GL_REPEAT;
		}
		Check(0);
	}
	GLenum GetGLFilterOptions(FilterOptions Filter , FilterOptions MipFilter = FO_NONE)
	{
		GLenum FinalOperation = 0;
		switch (Filter)
		{
		case FO_BILINEAR:
		case FO_TRILINEAR:
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			switch (MipFilter)
			{
			case FO_NONE: FinalOperation = GL_LINEAR; return FinalOperation;
			case FO_BILINEAR:
			case FO_TRILINEAR:
			case FO_ANISOTROPIC:
			case FO_LINEAR: FinalOperation = GL_LINEAR_MIPMAP_LINEAR; return FinalOperation;
			case FO_POINT: FinalOperation = GL_LINEAR_MIPMAP_NEAREST; return FinalOperation;
			}
			break;
		case FO_NONE:
		case FO_POINT:
			switch (MipFilter)
			{
			case FO_NONE: FinalOperation = GL_NEAREST;  return FinalOperation;
			case FO_BILINEAR:
			case FO_TRILINEAR:
			case FO_ANISOTROPIC:
			case FO_LINEAR: FinalOperation = GL_NEAREST_MIPMAP_LINEAR; return FinalOperation;
			case FO_POINT: FinalOperation = GL_NEAREST_MIPMAP_NEAREST; return FinalOperation;
			}
			break;
		}
		Check(0);
		return 0;
	}
	FORCEINLINE GLenum GetBlendEquation(SceneBlendOperation Equation)
	{
		switch (Equation)
		{
		case SBO_ADD: return GL_FUNC_ADD;
		case SBO_SUBTRACT: return GL_FUNC_SUBTRACT;
		case SBO_REVERSE_SUBTRACT: return GL_FUNC_REVERSE_SUBTRACT;
		case SBO_MIN: return GL_MIN;
		case SBO_MAX: return GL_MAX;
		}
		Check(0);
	}
	FORCEINLINE GLenum GetBlendFactor(SceneBlendFactor BlendFactor)
	{
		switch (BlendFactor)
		{
		case SBF_ONE:
			return GL_ONE;
		case SBF_ZERO:
			return GL_ZERO;
		case SBF_DEST_COLOUR:
			return GL_DST_COLOR;
		case SBF_SOURCE_COLOUR:
			return GL_SRC_COLOR;
		case SBF_ONE_MINUS_DEST_COLOUR:
			return GL_ONE_MINUS_DST_COLOR;
		case SBF_ONE_MINUS_SOURCE_COLOUR:
			return GL_ONE_MINUS_SRC_COLOR;
		case SBF_DEST_ALPHA:
			return GL_DST_ALPHA;
		case SBF_SOURCE_ALPHA:
			return GL_SRC_ALPHA;
		case SBF_ONE_MINUS_DEST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case SBF_ONE_MINUS_SOURCE_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		}
		return GL_ONE;
	}
}
