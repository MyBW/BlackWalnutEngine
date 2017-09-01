#pragma once
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
		switch (polygonMode)
		{
		case PM_SOLID:     return GL_FILL;
		case PM_POINTS:    return GL_POINT;
		case PM_WIREFRAME: return GL_LINE;
		}
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
		case CULL_ANTICLOCKWISE: return CL_BACK;
		}
	}

}
