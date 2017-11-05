#include "GLRenderSystem.h"
#include "GLTextureManager.h"
#include "GLTexture.h"
#include "GL/include/glew.h"
#include "../BWEngineCore/BWViewport.h"
#include "../BWEngineCore/BWRenderOperation.h"
#include "../BWEngineCore/BWVertexIndexData.h"
#include "../BWEngineCore/BWLog.h"
#include "../BWEngineCore/BWStringConverter.h"
#include"../BWEngineCore/BWHighLevelGpuProgramManager.h"
#include"../BWEngineCore/BWMaterialManager.h"
#include"../BWEngineCore/BWSceneManager.h"
#include"../BWEngineCore/BWRoot.h"
#include"../BWEngineCore/BWAutoParamDataSource.h"
#include"../BWEngineCore/BWDirectionalLight.h"
#include"../BWEngineCore/BWPointLight.h"
#include "GLHardwareVertexBuffer.h"
#include "GLHardwareIndexBuffer.h"
#include "GLHardwareBufferManager.h"
#include "GLHardwareDepthBuffer.h"
#include "GLContext.h"
#include "GLSupport.h"
#include "GLRenderTarget.h"
#include "GL/include/glut.h"
#include"GLPreDefine.h"
#include"GLHardwareVertexBuffer.h"
#include"GLHardwareIndexBuffer.h"
#include"GLRenderBuffer.h"

// 纯粹是为了测试纹理 
#include "GL/include/GLAUX.H"
#include "GLRenderState.h"
#include "GLHelper.h"
#include "GLPixelUtil.h"
#include "../BWEngineCore/BWRenderSystem.h"


#define  VBO_BUFFER_OFFSET(i) ((char*)NULL + (i))

bool GLRenderSystem::activateGLTextureUnit(size_t texUnit)
{
	if (mActiveTextureUnit != texUnit)
	{
		if (GLEW_VERSION_1_2 )
		{
			glActiveTexture(GL_TEXTURE0 + texUnit);
			mActiveTextureUnit = texUnit; 
			return true;
		}
		else if (!texUnit)
		{
			return true;
		}
		else
        {
			return false;
        }
	}
	else
	{
		return true;
	}
}
void GLRenderSystem::_setTexturelayerAnisotropy(size_t texUnit, unsigned int anisotropy)
{
	//assert(0);
}
void GLRenderSystem::_setPolygonMode(PolygonMode polygonMode)
{
	GLenum glModel;
	switch (polygonMode)
	{
	case PM_SOLID:
		glModel = GL_FILL;
		break;
	case PM_POINTS:
		glModel = GL_POINT;
		break;
	case PM_WIREFRAME:
		glModel = GL_LINE;
		break;
	}
	glPolygonMode( GL_FRONT_AND_BACK,glModel);
}
GLint GLRenderSystem::getCombineMinMipFilter() const
{
	switch (mMinFilterOperation)
	{
	case FO_ANISOTROPIC:
	case FO_LINEAR:
		switch (mMipFilterOperation)
		{
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		case FO_NONE:
			return GL_LINEAR;
		case FO_POINT:
			return GL_LINEAR_MIPMAP_NEAREST;
		default:
			break;
		}
		break;

	case FO_NONE:
	case FO_POINT:
		switch (mMipFilterOperation)
		{
		case FO_ANISOTROPIC:
		case FO_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		case FO_NONE:
			return GL_NEAREST;
		case FO_POINT:
			return GL_LINEAR_MIPMAP_NEAREST;
		}
		break;
	}
	assert(0);
	return 0;
}
void GLRenderSystem::_setTextureUnitFiltering(size_t texUnit, FilterType type, FilterOptions op)
{
	if (!activateGLTextureUnit(texUnit))
	{
		return;
	}
	switch (type)
	{
	case FT_MIN:
		mMinFilterOperation = op;
		glTexParameteri(mTextureType[texUnit], GL_TEXTURE_MIN_FILTER, getCombineMinMipFilter());
		break;
	case FT_MAG:
		switch (op)
		{
		case FO_LINEAR:
		case FO_ANISOTROPIC:
			glTexParameteri(mTextureType[texUnit], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			break;
		case FO_POINT:
		case FO_NONE:
			glTexParameteri(mTextureType[texUnit], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			break;
		}
		break;
	case FT_MIP:
		// 这里使用GL_LINEAR_MIPMAP_NEAREST 造成纹理题图不显示 也就是全部为白色 暂时使用GL_LINEAR
		mMipFilterOperation = op;
		glTexParameteri(mTextureType[texUnit], GL_TEXTURE_MIN_FILTER, GL_NEAREST); // getCombineMinMipFilter());
		break;
	}
	activateGLTextureUnit(0);
}
void GLRenderSystem::_setTexture(size_t texUnit, bool enable, const BWTexturePtr &textPtr)
{
	GLTexturePtr tex = textPtr;
	GLenum lastTextureType = mTextureType[texUnit];

	if (!activateGLTextureUnit(texUnit))
	{
		return;
	}
	if (enable)
	{
		if (!tex.IsNull())
		{
			tex->touch();
			mTextureType[texUnit] = tex->GetGLTextureTarget();
		}
		else
		{
			mTextureType[texUnit] = GL_TEXTURE_2D;
		}
		if (lastTextureType != mTextureType[texUnit] && lastTextureType != 0)
		{
			if (texUnit < mFixeFunctionTexutreUnits)
			{
				glDisable(lastTextureType);
			}
		}
		if (texUnit < mFixeFunctionTexutreUnits)
		{
			glEnable(mTextureType[texUnit]);
		}
		if (!tex.IsNull())
		{
			glBindTexture(mTextureType[texUnit], tex->GetHIID());
		}
		else
		{
			glBindTexture(mTextureType[texUnit], static_cast<GLTextureManager*>(mTextureManager)->getWarningTexture());
		}
	}
	else
	{
		if (texUnit < mFixeFunctionTexutreUnits)
		{
			if (lastTextureType != 0)
			{
				glDisable(mTextureType[texUnit]);
			}
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);//将该纹理的映射格式设置为正常格式  和 _setTextureCoordCalculation 正好相反
		}
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	activateGLTextureUnit(0);
}

void GLRenderSystem::clearFrameBuffer(unsigned int buffers, const ColourValue &color, float depth, unsigned short stencil)
{
	bool colourMaks = !mColourWrite[0] || !mColourWrite[1] || !mColourWrite[2] || !mColourWrite[3];
	unsigned int flag = 0;
	if (buffers & FBT_COLOUR)
	{
		flag |= GL_COLOR_BUFFER_BIT;
		if (colourMaks)
		{
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		}
		glClearColor(color.r, color.g, color.b, color.a);
	}
	if (buffers & FBT_DEPTH)
	{
		flag |= GL_DEPTH_BUFFER_BIT;
		if (!mDepthWrite)
		{
			glDepthMask(GL_TRUE);
		}
		glClearDepth(depth);
	}
	if (buffers & FBT_STENCIL)
	{
		flag |= GL_STENCIL_BUFFER_BIT;
		glStencilMask(0xffffffff);
		glClearStencil(stencil);
	}
	// 裁剪测试是否开启
	GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
	if (!scissorTestEnabled)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	GLint viewport[4], scissor[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetIntegerv(GL_SCISSOR_BOX, scissor);
	bool scissorboxDifference = viewport[0] != scissor[0] || viewport[1] != scissor[1] || viewport[2] != scissor[2] || viewport[3] != scissor[3];
	if (scissorboxDifference)
	{
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
	}

	glClear(flag);
	if (scissorboxDifference)
	{
		glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
	}
	if (!scissorTestEnabled)
	{
		glDisable(GL_SCISSOR_TEST);
	}
	if (!mDepthWrite && (buffers & FBT_DEPTH))
	{
		glDepthMask(GL_FALSE);
	}
	if (colourMaks && (buffers & FBT_COLOUR))
	{
		glColorMask(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
	}
	if (buffers & FBT_STENCIL)
	{
		glStencilMask(mStencilMask);
	}
}

void GLRenderSystem::setNormaliseNormals(bool enbale) const 
{
	if (enbale)
	{
		glEnable(GL_NORMALIZE);
	}
	else
	{
		glDisable(GL_NORMALIZE);
	}
}

void GLRenderSystem::_convertProjectionMatrix(const BWMatrix4& matrix, BWMatrix4 &dest, bool forGpuProgram)
{
	dest = matrix;
}
void GLRenderSystem::setShadingType(ShadeOptions type)
{
	switch (type)
	{
	case SO_FLAT:
		glShadeModel(GL_FLAT);
		break;
	case SO_GOURAUD:
	case  SO_PHONG:
		glShadeModel(GL_SMOOTH);
		break;
	}
}
GLint GLRenderSystem::convertCompareFun(CompareFunction function) const
{
	switch (function)
	{
	case CMPF_ALWAYS_FAIL:
		return GL_NEVER;
	case CMPF_ALWAYS_PASS:
		return GL_ALWAYS;
	case CMPF_LESS:
		return GL_LESS;
	case CMPF_LESS_EQUAL:
		return GL_LEQUAL;
	case CMPF_EQUAL:
		return GL_EQUAL;
	case CMPF_NOT_EQUAL:
		return GL_NOTEQUAL;
	case CMPF_GREATER_EQUAL:
		return GL_GEQUAL;
	case CMPF_GREATER:
		return GL_GREATER;
	}
	return GL_ALWAYS;
}
GLint GLRenderSystem::getBlendMode(SceneBlendFactor factor)
{
	switch (factor)
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
GLint GLRenderSystem::getTextureAddressingMode(BWTextureUnitState::TextureAddressingMode model)
{
	switch (model)
	{
	case BWTextureUnitState::TAM_WRAP:
		return GL_REPEAT;
	case BWTextureUnitState::TAM_MIRROR:
		return GL_MIRRORED_REPEAT;
	case BWTextureUnitState::TAM_CLAMP:
		return GL_CLAMP_TO_EDGE;
	case BWTextureUnitState::TAM_BORDER:
		return GL_CLAMP_TO_BORDER;
	}
}
void GLRenderSystem::_setCullingMode(CullingMode cullingMode)
{
	mCullingMode = cullingMode;

	// ogre 中规定 顺时针方向为正面 所以 在ogre 中不能调用glfrontface来改变这种约定
	GLenum glCullingMode;
	switch (cullingMode)
	{
	case CULL_NONE:
		glDisable(GL_CULL_FACE);
		return;
	case CULL_CLOCKWISE:
		if (mActiveRenderTarget && 
			((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
			(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding)))
		{
			glCullingMode = GL_FRONT;
		}
		else
		{
			glCullingMode = GL_BACK;
		}
		break;
	case CULL_ANTICLOCKWISE:
		if (mActiveRenderTarget &&
			((mActiveRenderTarget->requiresTextureFlipping() && !mInvertVertexWinding) ||
			(!mActiveRenderTarget->requiresTextureFlipping() && mInvertVertexWinding)))
		{
			glCullingMode = GL_BACK;
		}
		else
		{
			glCullingMode = GL_FRONT;
		}
		break;
	}
	glEnable(GL_CULL_FACE);
	glCullFace(glCullingMode);
}

void GLRenderSystem::_setColourBufferEnable(bool r, bool g, bool b, bool a)
{
	glColorMask(r, g, b, a);
	mColourWrite[0] = r;
	mColourWrite[1] = g;
	mColourWrite[2] = b;
	mColourWrite[3] = a;
}
void GLRenderSystem::_setAlphaRejectSetting(CompareFunction rejectFunction, unsigned char rejectValue, bool alphToCoverage)
{
	bool a2c = false;
	static bool lasta2c = false;
	if (rejectFunction == CMPF_ALWAYS_PASS)
	{
		glDisable(GL_ALPHA_TEST);
	}
	else
	{
		glEnable(GL_ALPHA_TEST);
		a2c = alphToCoverage;
		glAlphaFunc(convertCompareFun(rejectFunction), rejectValue / 255.0f);
	}
	//if (a2c != lasta2c&& getCapabilities()->hasCapabilities(rsc_alpha_to_coverage)) http://www.zwqxin.com/archives/opengl/talk-about-alpha-to-coverage.html
	{
		if (a2c)
		{
			glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}
		else
		{
			glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
		}
		lasta2c = a2c;
	}
}

void GLRenderSystem::_setDepthBias(float deptBias, float deptBiaseSlopScale)
{
	if (deptBias != 0 || deptBiaseSlopScale != 0)
	{
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_POLYGON_OFFSET_POINT);
		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-deptBiaseSlopScale, -deptBias);
	}
	else
	{
		glDisable(GL_POLYGON_OFFSET_LINE);
		glDisable(GL_POLYGON_OFFSET_POINT);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}
void GLRenderSystem::_setDepthBufferWriteEnable(bool enbale)
{
	GLboolean flag = enbale ? GL_TRUE : GL_FALSE;
	glDepthMask(flag);
	mDepthWrite = enbale;
}

void GLRenderSystem::_setDepthBufferCheckEnable(bool enable)
{
	if (enable)
	{
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
}
void GLRenderSystem::_setDepthBufferFunction(CompareFunction depthFunction)
{
	glDepthFunc(convertCompareFun(depthFunction));
}
void GLRenderSystem::_setSeparatesceneBlending(SceneBlendFactor sourceFactor, SceneBlendFactor destFactor, SceneBlendFactor sourceAlphaFactor, SceneBlendFactor destAlphaFactor, SceneBlendOperation op, SceneBlendOperation alphaOp)
{
	GLint sourceBlend = getBlendMode(sourceFactor);
	GLint desBlend = getBlendMode(destFactor);
	GLint sourceAlphaBlend = getBlendMode(sourceAlphaFactor);
	GLint desAlphaBlend = getBlendMode(destAlphaFactor);

	if (sourceFactor == SBF_ONE && destFactor == SBF_ZERO && 
		sourceAlphaFactor == SBF_ONE && destAlphaFactor == SBF_ZERO)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_BLEND);
		glBlendFuncSeparate(sourceBlend, desBlend, sourceAlphaFactor, desAlphaBlend);
	}

	GLint fun = GL_FUNC_ADD, alphFun = GL_FUNC_ADD;
	switch (op)
	{
	case SBO_SUBTRACT:
		fun = GL_FUNC_SUBTRACT;
		break;
	case SBO_REVERSE_SUBTRACT:
		fun = GL_FUNC_REVERSE_SUBTRACT;
		break;
	case SBO_MIN:
		fun = GL_MIN;
		break;
	case SBO_MAX:
		fun = GL_MAX;
		break;
	default:
		break;
	}
	switch (alphaOp)
	{
	case SBO_SUBTRACT:
		alphFun = GL_FUNC_SUBTRACT;
		break;
	case SBO_REVERSE_SUBTRACT:
		alphFun = GL_FUNC_REVERSE_SUBTRACT;
		break;
	case SBO_MIN:
		alphFun = GL_MIN;
		break;
	case SBO_MAX:
		alphFun = GL_MAX;
		break;
	default:
		break;
	}
	if (GLEW_VERSION_2_0)
	{
		glBlendEquationSeparate(fun, alphFun);
	}
	else
	{
		assert(0);
	}
}

void GLRenderSystem::_setSceneBlending(SceneBlendFactor sourceFactro, SceneBlendFactor destFactor, SceneBlendOperation op)
{
	GLint sourceBlend = getBlendMode(sourceFactro);
	GLint desrBlend = getBlendMode(destFactor);
	if (sourceFactro == SBF_ONE  && destFactor == SBF_ZERO)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_BLEND);
		glBlendFunc(sourceBlend, desrBlend);
	}
	GLint fun = GL_FUNC_ADD;
	switch (op)
	{
	case SBO_SUBTRACT:
		fun = GL_FUNC_SUBTRACT;
		break;
	case SBO_REVERSE_SUBTRACT:
		fun = GL_FUNC_REVERSE_SUBTRACT;
		break;
	case SBO_MIN:
		fun = GL_MIN;
		break;
	case SBO_MAX:
		fun = GL_MAX;
		break;
	default:
		break;
	}
	if (GLEW_VERSION_2_0)
	{
		glBlendEquation(fun);
	}
	else
	{
		assert(0);
	}
}

void GLRenderSystem::_setPointParameters(float size, bool attenuationEnable, float constant, float linear, float quadratic, float minSize, float maxSize)
{
	float val[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	if (attenuationEnable)
	{
		// 点的大小计算是像素级别的 同事也是和视口相关的  这里为了消除视口的影响
		size = size * mActiveVireport->getActualHeight();
		minSize = minSize * mActiveVireport->getActualWidth();
		if (maxSize == 0.0f)
		{
			 //maxSize = mCurrentCapabilites->getMaxPointSize();
			assert(0);
		}
		else
		{
			maxSize = maxSize * mActiveVireport->getActualWidth();
		}
		float correction = 0.005f;
		val[0] = constant;
		val[1] = linear * correction;
		val[2] = quadratic * correction;
		val[3] = 1;
		//if (mCurrentCapablities->hasCapability(rsc_vertex_program))
		{
			glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
		}
	}
	else
	{
		if (maxSize == 0.0f )
		{
			//maxSize = mCurrentCapabilites->getMaxPointSize();
			//assert(0);
			maxSize = 1.0f;
		}
		//if (mCurrentCapablities->hasCapability(rsc_vertex_program))
		{
			glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
		}
	}
	glPointSize(size);

	// 进行各种性能测试
	glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, val);
	glPointParameterf(GL_POINT_SIZE_MIN, minSize);
	glPointParameterf(GL_POINT_SIZE_MAX, maxSize);
}

void GLRenderSystem::_setPointSpriteEnable(bool enable)
{
    if (enable)
    {
		glEnable(GL_POINT_SPRITE);
    }
	else
	{
		glDisable(GL_POINT_SPRITE);
	}
	for (unsigned short i = 0; i < mFixeFunctionTexutreUnits; ++i)
	{
		activateGLTextureUnit(i);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE,
			enable ? GL_TRUE : GL_FALSE);
	}
	activateGLTextureUnit(0);
}

void GLRenderSystem::_setTextureMipmapBias(size_t texNum, float bias)
{
	if (activateGLTextureUnit(texNum))
	{
		glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, bias);
		activateGLTextureUnit(0);
	}
}
void GLRenderSystem::_setTextureBlendMode(size_t texNum,  const LayerBlendModeEx& model)
{
	if (texNum > mFixeFunctionTexutreUnits)
	{
		return;
	}
	//能力测试

	GLenum src1op, src2op, cmd;
	GLfloat cv1[4], cv2[4];
	if (model.blendType == LBT_COLOUR)
	{
		cv1[0] = model.colourArg1.r;
		cv1[1] = model.colourArg1.g;
		cv1[2] = model.colourArg1.b;
		cv1[3] = model.colourArg1.a;
		mManualBlendColours[texNum][0] = model.colourArg1;

		cv2[0] = model.colourArg2.r;
		cv2[1] = model.colourArg2.g;
		cv2[2] = model.colourArg2.b;
		cv2[3] = model.colourArg2.a;
		mManualBlendColours[texNum][1] = model.colourArg2;
	}
	if (model.blendType == LBT_ALPHA)
	{
		cv1[0] = mManualBlendColours[texNum][0].r;
		cv1[1] = mManualBlendColours[texNum][0].g;
		cv1[2] = mManualBlendColours[texNum][0].b;
		cv1[3] = model.alphaArg1;

		cv2[0] = mManualBlendColours[texNum][1].r;
		cv2[1] = mManualBlendColours[texNum][1].g;
		cv2[2] = mManualBlendColours[texNum][1].b;
		cv2[3] = model.alphaArg2;
	}
	switch (model.source1)
	{
	case  LBS_CURRENT:
		src1op = GL_PREVIOUS;
		break;
	case  LBS_TEXTURE:
		src1op = GL_TEXTURE;
		break;
	case LBS_MANUAL:
		src1op = GL_CONSTANT;
		break;
	case LBS_DIFFUSE:
		src1op = GL_PRIMARY_COLOR;
		break;
	case LBS_SPECULAR:
		src1op = GL_PRIMARY_COLOR;
		break;
	default:
		src1op = 0;
	}

	switch (model.source2)
	{
	case  LBS_CURRENT:
		src2op = GL_PREVIOUS;
		break;
	case  LBS_TEXTURE:
		src2op = GL_TEXTURE;
		break;
	case LBS_MANUAL:
		src2op = GL_CONSTANT;
		break;
	case LBS_DIFFUSE:
		src2op = GL_PRIMARY_COLOR;
		break;
	case LBS_SPECULAR:
		src2op = GL_PRIMARY_COLOR;
		break;
	default:
		src2op = 0;
	}
	switch (model.operation)
	{
	case  LBX_MODULATE:
		cmd = GL_MODULATE;
		break; 
	case LBX_SOURCE1:
		cmd = GL_REPLACE;
		break;
	case  LBX_SOURCE2:
	case  LBX_MODULATE_X2:
	case  LBX_MODULATE_X4:
		cmd = GL_REPLACE;
		break;
	case  LBX_ADD:
		cmd = GL_ADD;
		break;
	case  LBX_ADD_SIGNED:
		cmd = GL_ADD_SIGNED;
		break;
	case LBX_ADD_SMOOTH:
		cmd = GL_INTERPOLATE;
		break;
	case LBX_SUBTRACT:
		cmd = GL_SUBTRACT;
		break;
	case  LBX_BLEND_DIFFUSE_COLOUR:
		cmd = GL_INTERPOLATE;
		break;
	case LBX_BLEND_DIFFUSE_ALPHA:
	case LBX_BLEND_TEXTURE_ALPHA:
	case  LBX_BLEND_CURRENT_ALPHA:
		cmd = GL_INTERPOLATE;
		break;
	case LBX_DOTPRODUCT:
		cmd = GL_DOT3_RGB;
	default:
		cmd = 0;
		break;
	}
	if (!activateGLTextureUnit(texNum))
	{
		return;
	}
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	if (model.blendType == LBT_COLOUR)
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, cmd);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, src1op);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, src1op);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
	}
	else
	{
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, cmd);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, src1op);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, src1op);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_CONSTANT);
	}
	float blendValue[4] = { 0, 0, 0, model.factor };
	switch (model.operation)
	{
	case LBX_BLEND_DIFFUSE_COLOUR:
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PRIMARY_COLOR);
		break;
	case LBX_BLEND_DIFFUSE_ALPHA:
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PRIMARY_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PRIMARY_COLOR);
		break;
	case LBX_BLEND_TEXTURE_ALPHA:
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_TEXTURE);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_TEXTURE);
		break;
	case LBX_BLEND_CURRENT_ALPHA:
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_PREVIOUS);
		glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_PREVIOUS);
		break;
	case LBX_BLEND_MANUAL:
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, blendValue);
		break;
	default:
		break;
	};

	switch (model.operation)
	{
	case LBX_MODULATE_X2:
		glTexEnvi(GL_TEXTURE_ENV, model.blendType == LBT_COLOUR ?
		GL_RGB_SCALE : GL_ALPHA_SCALE, 2);
		break;
	case LBX_MODULATE_X4:
		glTexEnvi(GL_TEXTURE_ENV, model.blendType == LBT_COLOUR ?
		GL_RGB_SCALE : GL_ALPHA_SCALE, 4);
		break;
	default:
		glTexEnvi(GL_TEXTURE_ENV, model.blendType == LBT_COLOUR ?
		GL_RGB_SCALE : GL_ALPHA_SCALE, 1);
		break;
	}

	if (model.blendType == LBT_COLOUR){
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
		if (model.operation == LBX_BLEND_DIFFUSE_COLOUR){
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
		}
		else {
			glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
		}
	}

	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
	if (model.source1 == LBS_MANUAL)
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, cv1);
	if (model.source2 == LBS_MANUAL)
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, cv2);

	activateGLTextureUnit(0);
}

void GLRenderSystem::_setTextureBorderColour(size_t texNum, const ColourValue &color)
{
	if (activateGLTextureUnit(texNum))
	{
		GLfloat border[4] = { color.r, color.g, color.b, color.a };
		glTexParameterfv(mTextureType[texNum], GL_TEXTURE_BORDER_COLOR, border);
		activateGLTextureUnit(0);
	}
}

void GLRenderSystem::_setTextureAddressionMode(size_t texNum, const BWTextureUnitState::UVWAddressingMode &uvw)
{
	if (!activateGLTextureUnit(texNum))
	{
		return;
	}
	glTexParameteri(mTextureType[texNum], GL_TEXTURE_WRAP_S, getTextureAddressingMode(uvw.u));
	glTexParameteri(mTextureType[texNum], GL_TEXTURE_WRAP_T, getTextureAddressingMode(uvw.v));
	glTexParameteri(mTextureType[texNum], GL_TEXTURE_WRAP_R, getTextureAddressingMode(uvw.w));
	activateGLTextureUnit(0);
}
void GLRenderSystem::_setTextureCoordSet(size_t texUnit, unsigned int textureCoordSet)
{
	mTextureCoordIndex[texUnit] = textureCoordSet;
}

void GLRenderSystem::_setTextureCoordCalculation(size_t texNum, TexCoordCalcMethod texCoordCalcMethod, const BWFrustum * frustum)
{
	if (texNum > mFixeFunctionTexutreUnits)
	{
		return;
	}

	GLfloat M[16];
	BWMatrix4 projectionBias;
	mUseAutoTextureMatrix = false;
	GLfloat eyePlaneS[] = { 1.0, 0.0, 0.0, 0.0 };
	GLfloat eyePlaneT[] = { 0.0, 1.0, 0.0, 0.0 };
	GLfloat eyePlaneR[] = { 0.0, 0.0, 1.0, 0.0 };
	GLfloat eyePlaneQ[] = { 0.0, 0.0, 0.0, 1.0 };

	switch (texCoordCalcMethod)
	{
	case TEXCALC_NONE:
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		break;

	case TEXCALC_ENVIRONMENT_MAP:
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);

		// Need to use a texture matrix to flip the spheremap
		mUseAutoTextureMatrix = true;
		memset(mAutoTextureMatrix, 0, sizeof(GLfloat)* 16);
		mAutoTextureMatrix[0] = mAutoTextureMatrix[10] = mAutoTextureMatrix[15] = 1.0f;
		mAutoTextureMatrix[5] = -1.0f;

		break;

	case TEXCALC_ENVIRONMENT_MAP_PLANAR:
		// XXX This doesn't seem right?!
#ifdef GL_VERSION_1_3
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
#else
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
#endif
		break;
	case TEXCALC_ENVIRONMENT_MAP_REFLECTION:

		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);

		// We need an extra texture matrix here
		// This sets the texture matrix to be the inverse of the view matrix
		mUseAutoTextureMatrix = true;
		makeGLMatrix(M, mViewMatrix);

		// Transpose 3x3 in order to invert matrix (rotation)
		// Note that we need to invert the Z _before_ the rotation
		// No idea why we have to invert the Z at all, but reflection is wrong without it
		mAutoTextureMatrix[0] = M[0]; mAutoTextureMatrix[1] = M[4]; mAutoTextureMatrix[2] = -M[8];
		mAutoTextureMatrix[4] = M[1]; mAutoTextureMatrix[5] = M[5]; mAutoTextureMatrix[6] = -M[9];
		mAutoTextureMatrix[8] = M[2]; mAutoTextureMatrix[9] = M[6]; mAutoTextureMatrix[10] = -M[10];
		mAutoTextureMatrix[3] = mAutoTextureMatrix[7] = mAutoTextureMatrix[11] = 0.0f;
		mAutoTextureMatrix[12] = mAutoTextureMatrix[13] = mAutoTextureMatrix[14] = 0.0f;
		mAutoTextureMatrix[15] = 1.0f;

		break;
	case TEXCALC_ENVIRONMENT_MAP_NORMAL:
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);

		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glDisable(GL_TEXTURE_GEN_Q);
		break;
	case TEXCALC_PROJECTION_TEXTURE:
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS);
		glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT);
		glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR);
		glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		glEnable(GL_TEXTURE_GEN_R);
		glEnable(GL_TEXTURE_GEN_Q);

		mUseAutoTextureMatrix = true;

		// Set scale and translation matrix for projective textures
		projectionBias = BWMatrix4::CLIPSPACE2DTOIMAGESPACE;

		projectionBias = projectionBias * frustum->getProjectionMatrix();
		if (mTexProjRelative)
		{
			BWMatrix4 viewMatrix;
			frustum->calcViewMatrixRelative(mTexProjRelativeOrigin, viewMatrix);
			projectionBias = projectionBias * viewMatrix;
		}
		else
		{
			projectionBias = projectionBias * frustum->getViewMatrix();
		}
		projectionBias = projectionBias * mWorldMatrix;

		makeGLMatrix(mAutoTextureMatrix, projectionBias);
		break;
	default:
		break;
	}
	activateGLTextureUnit(0);
	
}
void GLRenderSystem::makeGLMatrix(GLfloat gl_matrix[16], const BWMatrix4& m)
{
	size_t x = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			gl_matrix[x] = m[j][i];
			x++;
		}
	}
}

void GLRenderSystem::_setTextureMatrix(size_t texNum, const BWMatrix4 &matrix)
{
	if (!activateGLTextureUnit(texNum) || texNum >= mFixeFunctionTexutreUnits)
	{
		return;
	}
	GLfloat mat[16];
	makeGLMatrix(mat, matrix);

	glMatrixMode(GL_TEXTURE);

	glLoadMatrixf(mat);

	if (mUseAutoTextureMatrix)
	{
		glMultMatrixf(mAutoTextureMatrix);
	}
	glMatrixMode(GL_MODELVIEW);
	activateGLTextureUnit(0);
}

void GLRenderSystem::_setWorldMatrices(const BWMatrix4* m, unsigned short count)
{
	// 一直没有被用到啊。。。。
	_setWorldMatrix(BWMatrix4::IDENTITY);
}

void GLRenderSystem::_setWorldMatrix(const BWMatrix4 &m)
{
	GLfloat mat[16];
	mWorldMatrix = m;
	makeGLMatrix(mat, mViewMatrix * mWorldMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mat);
}
void GLRenderSystem::_setViewMatrix(const BWMatrix4 &m)
{
	mViewMatrix = m;
	GLfloat mat[16];
	makeGLMatrix(mat, mViewMatrix * mWorldMatrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(mat);
	//if (!mClipPlane.empty())
	{
		mClipPlanesDirty = true;
	}
}
void  GLRenderSystem::_setProjectionMatrix(const BWMatrix4 & matrix)
{
	GLfloat mat[16];
	makeGLMatrix(mat, matrix);
	if (mActiveRenderTarget->requiresTextureFlipping())
	{
		mat[1] = -mat[1];
		mat[5] = -mat[5];
		mat[9] = -mat[9];
		mat[13] = -mat[13];

	}
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(mat);
	glMatrixMode(GL_MODELVIEW);
	//if (!mClipPlane.empty())
	{
		mClipPlanesDirty = true;
	}
}
void GLRenderSystem::_setViewport(BWViewport *viewport)
{
	if (viewport != mActiveVireport || viewport->_isUpdated())
	{
		BWRenderTarget *target;
		target = viewport->getRenderTarget();
		_setRenderTarget(target);
		mActiveVireport = viewport;
		GLsizei x, y, w, h;
		w = viewport->getActualWidth();
		h = viewport->getActualHeight();
		x = viewport->getActualLeft();
		y = viewport->getActualTop();
		if (!target->requiresTextureFlipping())
		{
			y = target->getHeight() - h - y;
		}
		glViewport(x, y, w, h);
		glScissor(x, y, w, h);
		viewport->_clearUpdateFlag();
	}
}
void GLRenderSystem::_switcheContext(GLContext *context)
{
	// 这里解绑定各种GPU程序 为什么呢？
	
	// 这里将灯光清空

	// 关闭纹理单元

	if (mCurrentContext)
	{
		mCurrentContext->endCurrent();
	}
	mCurrentContext = context;
	mCurrentContext->setCurrent();

	if (!mCurrentContext->getInitialized())
	{
		_oneTimeContextInitialization();
		mCurrentContext->setInitialized();
	}
	//再次绑定各种GPU


	glDepthMask(mDepthWrite);
	glColorMask(mColourWrite[0], mColourWrite[1], mColourWrite[2], mColourWrite[3]);
	glStencilMask(mStencilMask);
}
void GLRenderSystem::_setRenderTarget(BWRenderTarget *renderTarget)
{
	if (mActiveRenderTarget)
	{
		// 这里需要解绑定 暂时不处理
	}
	mActiveRenderTarget = renderTarget;
	GLContext * newContext = 0;
	renderTarget->getCustomAttribute("GLCONTEXT", &newContext);
	if (newContext && mCurrentContext != newContext)
	{
		// 这个函数里面处理的过程很诡异啊
		_switcheContext(newContext);
	}
	//绑定当前renderTarget
	if (GLEW_EXT_framebuffer_sRGB)
	{
	    if (renderTarget->isHardwareGammaEnabled())
	    {
			glEnable(GL_FRAMEBUFFER_SRGB);
	    }
		else
		{
			glDisable(GL_FRAMEBUFFER_SRGB);
		}
	}
}

void GLRenderSystem::initialiseContext(BWRenderWindow *primary)
{
	mMainContext = 0;
	primary->getCustomAttribute("GLCONTEXT", &mMainContext);
	mCurrentContext = mMainContext;

	if (mCurrentContext)
	{
		mCurrentContext->setCurrent();
	}

	mGLSupport->initialiseExtension();
	glewInit();
}


BWRenderWindow* GLRenderSystem::_initialise(bool autoCreateWindow, const std::string &windowTitle)
{
	mGLSupport->start();
	BWRenderWindow *renderWindow = mGLSupport->createWindow(autoCreateWindow, this, windowTitle);
	BWRenderSystem::_initialise(autoCreateWindow, windowTitle);
	return renderWindow;
}
void GLRenderSystem::unbindGpuProgram(GpuProgramType GPT)
{
	BWRenderSystem::unbindGpuProgram(GPT);
	glUseProgram(0);
	glBindVertexArray(0);
	mGPUProgram.SetNull();
}
BWRenderWindow* GLRenderSystem::_createRenderWindow(const std::string &name, unsigned int width, unsigned int height, bool fullScreen, const NameValuePairList* miscParam)
{
	if (mRenderTargetMap.find(name) != mRenderTargetMap.end())
	{
		assert(0);
	}
	BWRenderWindow *win = mGLSupport->newWindow(name, width, height, fullScreen, miscParam);
	win->setPriority(OGRE_WINDOW_RENDERTARGET_GROUPS);
	attacheRenderTarget(win);
	if (!mGLInitialised)
	{
		initialiseContext(win);
		
		StringVector tokens = StringUtil::Split(mGLSupport->getGLVersioin(), ".");
		if (!tokens.empty())
		{
			// 设置各种能力等等
		}
		//创建能力
		if (!mUseCustomCapabilities)
		{
			//设置当前能力
		}
		initialiseFromRenderSystemCapabilities();
		_oneTimeContextInitialization();
		if (mCurrentContext)
		{
			mCurrentContext->setInitialized();
		}
	}
	return win;
}

void GLRenderSystem::_oneTimeContextInitialization()
{
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR); //先进行纹理计算 在将高光加入到纹理中
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1);

	glEnable(GL_COLOR_SUM); //不知道什么意思 估计现在的流水线都不用了吧 回头看看
	glDisable(GL_DITHER);// 关闭抖动算法

	glEnable(GL_MULTISAMPLE); //开启多重采样
}
void GLRenderSystem::initConfigOptions()
{
	mGLSupport->addConfig();
}
void GLRenderSystem::initialiseFromRenderSystemCapabilities()
{
	//依据capabilitiess来设定rendersystem

	mFixeFunctionTexutreUnits = 32;
	GLint maxTextureCoord;
	glGetIntegerv(GL_MAX_TEXTURE_COORDS, &maxTextureCoord);
	mFixeFunctionTexutreUnits = maxTextureCoord;

	glewInit();
	
    // 在这里设定各种和rendersystem 相关的管理类 例如各种buffer  各种GPU程序管理等等

	mGLInitialised = true;
}
void GLRenderSystem::_render(const BWRenderOperation &ro)
{
	BWRenderSystem::_render(ro);

	if (CachedPipelineState.GPUProgramUsage.Get())
	{
		BWHighLevelGpuProgramPtr GPUProgram = CachedPipelineState.GPUProgramUsage->GetHighLevelGpuProgram();
		GLSLGpuProgram* GLGPUProgram = dynamic_cast<GLSLGpuProgram*>(GPUProgram.Get());
		GLGPUProgram->bind();
		GPUProgram->SetGPUProgramParameters(CachedPipelineState.GPUProgramUsage->GetGpuProgramParameter());
		GLGPUProgram->bindingBuffer(ro.indexData, ro.vertexData);


		GLint primType;
		bool useAdjacenty = false;
		GLenum indexType = (ro.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
		switch (ro.operationType)
		{
		case  BWRenderOperation::OT_POINT_LIST:
			primType = GL_POINT;
			break;
		case BWRenderOperation::OT_LINE_LIST:
			primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
			break;
		case  BWRenderOperation::OT_LINE_STRIP:
			primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
			break;
		case  BWRenderOperation::OT_TRIANGLE_LIST:
			primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
			break;
		case  BWRenderOperation::OT_TRIANGLE_STRIP:
			primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
		case BWRenderOperation::OT_TRIGANLE_FAN:
			primType = GL_TRIANGLE_FAN;
		default:
			break;
		}
		glDrawElements(primType, ro.indexData->mIndexCount, indexType, NULL);
		unbindGpuProgram(GPT_GPU_PROGRAM);
		return;
	}
	//!!!!!!!!!!!!!!! 不要把可编程管线和固定管线混合使用 会出现未定义行为  例如 深度图失效
	return;
   //固定渲染管线前 一定要把gup 程序清零
	unbindGpuProgram(GPT_GPU_PROGRAM);
	void *pBufferData = 0;
	bool multitextureing = true;

	const  VertexDeclearaion::VertexElementList decl =
		ro.vertexData->mVertexDeclaration->getElements();
	VertexDeclearaion::VertexElementList::const_iterator element, elementEnd;
	elementEnd = decl.end();
	element = decl.begin();
	std::vector<GLuint> attribBound; //  这个里面记录了和GPU程序交互的各种属性变量
	while (element != elementEnd)
	{
		if (!ro.vertexData->mVertexBufferBind->isBufferBound(element->getSource()))
		{
			continue; 
		}
		BWHardwareVertexBufferPtr vertexBuffer =
			ro.vertexData->mVertexBufferBind->getBuffer(element->getSource());
		if (true) // 这里测试是否支持 VBO
		{
			glBindBuffer(GL_ARRAY_BUFFER, static_cast<const GLHardwareVertexBuffer*>(vertexBuffer.Get())->getGLHardwareBufferID());
			pBufferData = VBO_BUFFER_OFFSET(element->getOffset());
		}
		else
		{
			assert(0);
		}
	    if (ro.vertexData->mVertexStart)
	    {
			pBufferData = static_cast<char*>(pBufferData)+ro.vertexData->mVertexStart*vertexBuffer->getVertexSize();
	    }
		unsigned int i = 0;
		VertexElementSemantic sem = element->getVertexSemantic();
		bool isCoustumAtrribute = false;
		/*if (mCurrentVertexProgram)
		{
			isCoustumAtrribute = mCurrentVertexProgram->isAtrributeValid(sem, element->getIndex());
		}*/
		
		if (isCoustumAtrribute)
		{
			//在这里设置attriBound
			//设置各种shader相关的东西
			assert(0);
		}
		else
		{
			
			//固定管线渲染
			switch (sem)
			{
			case VES_POSITION:
				glVertexPointer(
					VertexElement::getTypeCount(element->getType()),
					GLHardwareBufferManager::getGLType(element->getType()),
					static_cast<GLsizei>(vertexBuffer->getVertexSize()),
					pBufferData
					);
				glEnableClientState(GL_VERTEX_ARRAY);
				
				break;
			case VES_NORMAL:
				glNormalPointer(
					GLHardwareBufferManager::getGLType(element->getType()),
					static_cast<GLsizei>(vertexBuffer->getVertexSize()),
					pBufferData
					);
				glEnableClientState(GL_NORMAL_ARRAY);
				break;
			case VES_DIFFUSE:
				glColorPointer(
					4,
					GLHardwareBufferManager::getGLType(element->getType()),
					static_cast<GLsizei>(vertexBuffer->getVertexSize()),
					pBufferData
					);
				glEnableClientState(GL_COLOR_ARRAY);
				
				break;
			case VES_SPECULAR:
				glSecondaryColorPointer(
					4,
					GLHardwareBufferManager::getGLType(element->getType()),
					vertexBuffer->getVertexSize(),
					pBufferData
					);
			
				glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
				
			case VES_TEXTURE_COORDINATES:
				if (false) // 当前是否使用shader
				{
					assert(0);
				}
				else
				{
					for (int i = 0; i < mDisableTexUnitFrom; i++)
					{
						if (multitextureing)
						{
							glClientActiveTexture(GL_TEXTURE0 + i);
						}
						glTexCoordPointer(
							VertexElement::getTypeCount(element->getType()),
							GLHardwareBufferManager::getGLType(element->getType()),
							static_cast<GLsizei>(vertexBuffer->getVertexSize()),
							pBufferData
							);
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					}
				}
				break;
			}
		}
		element++;
	}
	if (multitextureing)
	{
		glClientActiveTexture(GL_TEXTURE0);
	}
	GLint primType;
	bool useAdjacenty = false;//(mGeometryProgramBound && mcurrentGeometryProgram->isAdjacencyInforRequired());
	switch (ro.operationType)
	{
	case  BWRenderOperation::OT_POINT_LIST:
		primType = GL_POINT;
		break;
	case BWRenderOperation::OT_LINE_LIST:
		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
		break;
	case  BWRenderOperation::OT_LINE_STRIP:
		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
		break;
	case  BWRenderOperation::OT_TRIANGLE_LIST:
		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		break;
	case  BWRenderOperation::OT_TRIANGLE_STRIP:
		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
	case BWRenderOperation::OT_TRIGANLE_FAN:
		primType = GL_TRIANGLE_FAN;
	default:
		break;
	}

	if (ro.useIndexes)
	{
		if (true) // 是否支持VBO
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
				static_cast<GLHardwareIndexBuffer*>(
				ro.indexData->mIndexBuffer.Get())->getGLBufferID());
			pBufferData = VBO_BUFFER_OFFSET(
				ro.indexData->mIndexStart *ro.indexData->mIndexBuffer->getIndexSize()
				);
		}
		else
		{
			assert(0);
		}
		GLenum indexType = (ro.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;

		do 
		{
			if (mDerivedDepthBisa && mCurrentPassIterationNum > 0)
			{
				_setDepthBias(mDerivedDepthBisaBase + mDerivedDepthBisaMultiplier * mCurrentPassIterationNum,
					mDerivedDepthBisaSlopeScale);

			}
			 //  ！！！！！！！！！！！！！！注意 这个就是真正绘制的函数
			glDrawElements(primType, ro.indexData->mIndexCount, indexType, pBufferData);
		} while (updatePassIterationRenderState()); // 这个里面更新shader 
	}
	else
	{
	    do{
	    	if (mDerivedDepthBisa && mCurrentPassIterationNum > 0)
	    	{
	    		_setDepthBias(mDerivedDepthBisaBase + mDerivedDepthBisaMultiplier * mCurrentPassIterationNum,
	    			mDerivedDepthBisaSlopeScale);
	    
	    	}
	    	//  ！！！！！！！！！！！！！！注意 这个就是真正绘制的函数
	    	glDrawArrays(primType, 0, ro.vertexData->mVertexCount);
		} while (updatePassIterationRenderState()); // 这个里面更新shader 
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);
	if (multitextureing)
	{
		for (int i = 0; i < mFixeFunctionTexutreUnits; i++)
		{
			glClientActiveTexture(GL_TEXTURE0 + i);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
		glClientActiveTexture(GL_TEXTURE0); 
	}
	else
	{
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
	for (std::vector<GLuint>::iterator ai = attribBound.begin(); ai != attribBound.end(); ai++)
	{
		glDisableVertexAttribArray(*ai);
	}
	glColor4f(1, 1, 1, 1);
	glSecondaryColor3f(0, 0, 0);
	unbindGpuProgram(GPT_VERTEX_PROGRAM);
}

BWRenderTarget* GLRenderSystem::createRenderTarget(const std::string &name)
{
	if (mRenderTargetMap.find(name) != mRenderTargetMap.end())
	{
		Log::GetInstance()->logMessage("GLRenderSystem::createRnederTarget() : Error");
		assert(0);
	}
	BWRenderTarget* renderTarget = new GLRenderTarget();
	return renderTarget;
}

void GLRenderSystem::DestroyRenderTarget(const std::string &name)
{
	if (mRenderTargetMap.find(name) != mRenderTargetMap.end())
	{

	}
}

void GLRenderSystem::bindGpuProgram(BWGpuProgram *GpuProgamr)
{
	assert(0);
	// 绑定操作
}
//void GLRenderSystem::bindGPUProgramUsage(BWGpuProgramUsage*gpuPrgramUsage)
//{
//	if (gpuPrgramUsage)
//	{
//		if (mGPUProgram.Get())
//		{
//			unbindGpuProgram(mGPUProgram->GetType());
//		}
//		mGPUProgram = gpuPrgramUsage->GetGpuProgram();
//		GLSLGpuProgram* glslProgram = dynamic_cast<GLSLGpuProgram*>(mGPUProgram.Get());
//		glslProgram->bind();
//		// 设置各种renderbuffer  这里是设置延迟渲染的Buffer  如果想改成非延时渲染 这里使用
//		//GLRenderTarget* renderTarget = dynamic_cast<GLRenderTarget*>(mActiveRenderTarget);
//		GLRenderTarget* renderTarget = dynamic_cast<GLRenderTarget*>(GRenderTarget);
//		NameLocation fragmentOutNameLocation = glslProgram->getFragmentOutNameLocation();
//		renderTarget->setDrawBuffers(fragmentOutNameLocation);
//		glslProgram->SetGPUProgramParameters(gpuPrgramUsage->GetGpuProgramParameter());
//	}
//}
void GLRenderSystem::GLtranspose(GpuConstantType type, void *data) 
{
	switch (type)
	{
	case GCT_MATRIX_2X2:
	case GCT_MATRIX_2X3:
	case GCT_MATRIX_2X4:
	case GCT_MATRIX_3X2:
	case GCT_MATRIX_3X3:
	case GCT_MATRIX_3X4:
	case GCT_MATRIX_4X2:
	case GCT_MATRIX_4X3:
		assert(0);
		return;
	case GCT_MATRIX_4X4:
	{
						   float *tmpData = (float*)(data);
						   for (int i = 0; i < 4; i++)
						   {
							   for (int j = i; j < 4; j++)
							   {
								   float a = tmpData[i * 4 + j];
								   tmpData[i * 4 + j] = tmpData[j * 4 + i];
								   tmpData[j * 4 + i] = a;
							   }
						   }
						   return;
	}
	
	}
	assert(0);
}

bool GLRenderSystem::InitRendererResource()
{
	
	if (BWRenderSystem::InitRendererResource())
	{
		return true;
		BWPass* pass = mDirectionLightM->getTechnique(0)->GetPass(0);
		DirectLightGLSLPrgramUsage = pass->getGPUProgramUsage();
		DirectLightGLSLProgram = dynamic_cast<GLSLGpuProgram*>(DirectLightGLSLPrgramUsage->GetGpuProgram().Get());
		DirectLightGLSLProgram->bind();
		DirectLightShadowMapLoc = glGetUniformLocation(DirectLightGLSLProgram->getID(), "ShadowMap");
		pass = mPointLightM->getTechnique(0)->GetPass(0);
		mPointLightDefferLightingGpuPrgramUsage = pass->getGPUProgramUsage();
		mPointLightDefferLightingGLSLProgram = dynamic_cast<GLSLGpuProgram*>(mPointLightDefferLightingGpuPrgramUsage->GetGpuProgram().Get());
		mPointLightDefferLightingGLSLProgram->bind();
		
		BWMeshPrt  mPointMesh = getPointLightMesh();
		BWSubMesh * mPointSubMesh = mPointMesh->getSubMesh(0);
		mPointSubMesh->_getRenderOperation(SphereMeshRenderOperation);
		GLABufferTexture = ABufferTexture;
		GLNormalTexture = BBufferTexture;
		GLPositionTexture = CBufferTexture;
		GLGBuffer = dynamic_cast<GLRenderTarget*>(GRenderTarget);
		FinalRenderResult = dynamic_cast<GLHardwarePixelBuffer*>(GLGBuffer->getPixelRenderBuffer(std::string("FinalRenderResult")).Get());
		//shadowmap相关shader
	/*	pass = DirectionLightShadowMapM->getTechnique(0)->GetPass(0);
		DirectionalLightShadowMapGpuPrgramUsage = pass->getGPUProgramUsage();
		DirectionalLightShadowMapGLSLProgram = dynamic_cast<GLSLGpuProgram*>(DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgram().Get());
		DirectionalLightShadowMapGLSLProgram->bind();
		pass = PointLightShadowMapM->getTechnique(0)->GetPass(0);
		PointLightShadowMapGpuPrgramUsage = pass->getGPUProgramUsage();
		PointLightShadowMapGLSLProgram = dynamic_cast<GLSLGpuProgram*>(PointLightShadowMapGpuPrgramUsage->GetGpuProgram().Get());
		PointLightShadowMapGLSLProgram->bind();
		GLShadowMapRenderTarget = dynamic_cast<GLRenderTarget*>(ShadowMapTarget);*/

		
		//GLSkyBoxTexture = mSkyBoxM->getTechnique(0)->GetPass(0)->getTextureUnitState(0)->_getTexturePtr();

		
		
	

	/*	pass = AmbientOcclusionMaterial->getTechnique(0)->GetPass(0);
		AmbientOcclusionGpuPrgramUsage = pass->getGPUProgramUsage();
		AmbientOcclusionProgram = dynamic_cast<GLSLGpuProgram*>( AmbientOcclusionGpuPrgramUsage->GetGpuProgram().Get());
		
		pass = AmbientOcclusionMaterial->getTechnique(0)->GetPass(1);
		AmbientOcclusionFilterGpuPrgramUsage = pass->getGPUProgramUsage();
		AmbientOcclusionFilterProgram = dynamic_cast<GLSLGpuProgram*>(AmbientOcclusionFilterGpuPrgramUsage->GetGpuProgram().Get());*/

		return true;
	}
	return false;
}

void GLRenderSystem::SetViewport(int ViewportX, int ViewportY, int ViewportWidth, int ViewportHight)
{
	glViewport(ViewportX, ViewportY, ViewportWidth, ViewportHight);
}


void GLRenderSystem::SetScissor(bool IsEnable, int ScissorX /*= 0 */, int ScissorY /*= 0*/, int ScissorWidth /*= 0 */, int ScissorHigh /*= 0 */)
{
	if (IsEnable) glEnable(GL_SCISSOR_TEST);
	else glDisable(GL_SCISSOR_TEST);
	glScissor(ScissorX, ScissorY, ScissorWidth, ScissorHigh);
}

void GLRenderSystem::SetRenderTarget(BWGpuProgramUsagePtr GPUProgramUsage, RSRenderTarget& InRenderTarget, BWHardwareDepthBufferPtr InDepthBuffer)
{
	if (InRenderTarget.RenderTargetTexture.Get() &&
	    InDepthBuffer.Get())
	{
		GLint CurrentFrameBuffer;
		GLTexture* RenderTexuture = dynamic_cast<GLTexture*>(InRenderTarget.RenderTargetTexture.Get());
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
		if (CurrentFrameBuffer)
		{
			GLuint OldFrameBuffer = GLuint(CurrentFrameBuffer);
			glDeleteFramebuffers(1, &OldFrameBuffer);
		}

		GLuint NewFrameBuffer = 0;
		static GLuint tmpFrameBuffferID = 0; //目前还没有FrameBuffer的管理机制 暂时这里处理
		glDeleteFramebuffers(1, &tmpFrameBuffferID);
		glGenFramebuffers(1, &NewFrameBuffer);
		tmpFrameBuffferID = NewFrameBuffer;

		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer));
		GLint TextureID = RenderTexuture->GetHIID();
		TextureType RenderTextureType = RenderTexuture->GetTextureType();
		int Index = InRenderTarget.Index;
		int MipLevel = InRenderTarget.MipmapLevel;
		if (RenderTextureType == TEX_TYPE_2D)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, MipLevel));  //暂时使用2D纹理
		}
		if (RenderTextureType == TEX_TYPE_CUBE_MAP)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, TextureID, MipLevel));
		}
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		GLHardwareDepthBuffer* DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>(InDepthBuffer.Get());
		GLint DepthID = DepthBuffer->GetHIID();
		GLenum DepthAttachment = DepthBuffer->GetAttachment();;
		glFramebufferTexture2D(GL_FRAMEBUFFER, DepthAttachment, GL_TEXTURE_2D, DepthID, 0);
		GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (statues != GL_FRAMEBUFFER_COMPLETE)//某个绑定到attachment上的元素无效
		{
			Log::GetInstance()->logMessage("GLTexture::attachToRenderTarget() : Error");
		}
	}
}


void GLRenderSystem::SetRenderTarget(BWGpuProgramUsagePtr GPUProgramUsage, RSRenderTarget& InRenderTarget)
{
	if (InRenderTarget.RenderTargetTexture.Get() &&
		InRenderTarget.DepthBuffer.Get())
	{
		GLint CurrentFrameBuffer;
		GLTexture* RenderTexuture = dynamic_cast<GLTexture*>(InRenderTarget.RenderTargetTexture.Get());
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
		if (CurrentFrameBuffer)
		{
			GLuint OldFrameBuffer = GLuint(CurrentFrameBuffer);
			glDeleteFramebuffers(1, &OldFrameBuffer);
		}

		GLuint NewFrameBuffer = 0;
		static GLuint tmpFrameBuffferID = 0; //目前还没有FrameBuffer的管理机制 暂时这里处理
		glDeleteFramebuffers(1, &tmpFrameBuffferID);
		glGenFramebuffers(1, &NewFrameBuffer);
		tmpFrameBuffferID = NewFrameBuffer;

		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer));
		GLint TextureID = RenderTexuture->GetHIID();
		TextureType RenderTextureType = RenderTexuture->GetTextureType();
		int Index = InRenderTarget.Index;
		int MipLevel = InRenderTarget.MipmapLevel;
		if (RenderTextureType == TEX_TYPE_2D)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, MipLevel));  //暂时使用2D纹理
		}
		if (RenderTextureType == TEX_TYPE_CUBE_MAP)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, TextureID, MipLevel));
		}
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		GLHardwareDepthBuffer* DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>(InRenderTarget.DepthBuffer.Get());
		GLint DepthID = DepthBuffer->GetHIID();
		GLenum DepthAttachment = DepthBuffer->GetAttachment();;
		glFramebufferTexture2D(GL_FRAMEBUFFER, DepthAttachment, GL_TEXTURE_2D, DepthID, InRenderTarget.DepthMipmapLevel);
		GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (statues != GL_FRAMEBUFFER_COMPLETE)//某个绑定到attachment上的元素无效
		{
			Log::GetInstance()->logMessage("GLTexture::attachToRenderTarget() : Error");
		}
	}
}

void GLRenderSystem::SetRenderTargets(RSRenderTargets& InRenderTargets)
{
	GLint CurrentFrameBuffer;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
	if (CurrentFrameBuffer)
	{
		GLuint OldFrameBuffer = GLuint(CurrentFrameBuffer);
		glDeleteFramebuffers(1, &OldFrameBuffer);
	}
	std::vector<GLenum> DrawBufferEnum;
	BWGpuProgramPtr mGPUProgram = InRenderTargets.GPUProgramUsage->GetGpuProgram();
	GLSLGpuProgram* GLSLProgram = dynamic_cast<GLSLGpuProgram*>(mGPUProgram.Get());
	NameLocation FragmentOutNameLocation = GLSLProgram->getFragmentOutNameLocation();
	static GLuint TmpFrameBuffer = 0;
	GLuint NewFrameBuffer = 0;
	glDeleteFramebuffers(1, &TmpFrameBuffer);
	glGenFramebuffers(1, &NewFrameBuffer);
	TmpFrameBuffer = NewFrameBuffer;
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer));

	NameLocation::iterator NameLocItor = FragmentOutNameLocation.begin();
	for (; NameLocItor != FragmentOutNameLocation.end(); NameLocItor++ )
	{
		RSRenderTarget* RenderTarget = nullptr;
		for (int TexutureIndex = 0; TexutureIndex < InRenderTargets.RenderTargets.size(); TexutureIndex++)
		{
			if (InRenderTargets.RenderTargets[TexutureIndex].RenderTargetTexture->getName()
				== NameLocItor->first)
			{
				RenderTarget = &InRenderTargets.RenderTargets[TexutureIndex];
				break;
			}
		}
		if (RenderTarget)
		{
			GLTexture* RenderTexuture = dynamic_cast<GLTexture*>(RenderTarget->RenderTargetTexture.Get());
			GLint TextureID = RenderTexuture->GetHIID();
			TextureType RenderTextureType = RenderTexuture->GetTextureType();
			int Index = RenderTarget->Index;
			int MipLevel = RenderTarget->MipmapLevel;
			if (RenderTextureType == TEX_TYPE_2D)
			{
				CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + NameLocItor->second, GL_TEXTURE_2D, TextureID, MipLevel));  //暂时使用2D纹理
			}
			if (RenderTextureType == TEX_TYPE_CUBE_MAP)
			{
				CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + NameLocItor->second, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, TextureID, MipLevel));
			}
			DrawBufferEnum.push_back(GL_COLOR_ATTACHMENT0 + NameLocItor->second);
		}
	}
	CHECK_GL_ERROR(glDrawBuffers(DrawBufferEnum.size(),DrawBufferEnum.data()));
	
	GLHardwareDepthBuffer* DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>(InRenderTargets.DepthAndStencil.Get());
	GLint DepthID = DepthBuffer->GetHIID();
	GLenum DepthAttachment = DepthBuffer->GetAttachment();;
	glFramebufferTexture2D(GL_FRAMEBUFFER, DepthAttachment, GL_TEXTURE_2D, DepthID, 0);
	GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (statues != GL_FRAMEBUFFER_COMPLETE)//某个绑定到attachment上的元素无效
	{
		Log::GetInstance()->logMessage("GLTexture::attachToRenderTarget() : Error");
	}
}

void GLRenderSystem::SetRenderTargetImmediately(RSRenderTarget& InRenderTarget)
{
	if (InRenderTarget.RenderTargetTexture.Get())
	{
		GLint CurrentFrameBuffer;
		GLTexture* RenderTexuture = dynamic_cast<GLTexture*>(InRenderTarget.RenderTargetTexture.Get());
		glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);
		if (CurrentFrameBuffer)
		{
			GLuint OldFrameBuffer = GLuint(CurrentFrameBuffer);
			glDeleteFramebuffers(1, &OldFrameBuffer);
		}

		GLuint NewFrameBuffer = 0;
		glGenFramebuffers(1, &NewFrameBuffer);
		CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer));
		GLint TextureID = RenderTexuture->GetHIID();
		TextureType RenderTextureType = RenderTexuture->GetTextureType();
		int Index = InRenderTarget.Index;
		int MipLevel = InRenderTarget.MipmapLevel;
		if (RenderTextureType == TEX_TYPE_2D)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, MipLevel));  //暂时使用2D纹理
		}
		if (RenderTextureType == TEX_TYPE_CUBE_MAP)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, TextureID, MipLevel));
		}
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		//GLHardwareDepthBuffer* DepthBuffer = dynamic_cast<GLHardwareDepthBuffer*>(InRenderTarget.DepthAndStencil.Get());
		//GLint DepthID = DepthBuffer->GetHIID();
		//GLenum DepthAttachment = DepthBuffer->GetAttachment();;
		//glFramebufferTexture2D(GL_FRAMEBUFFER, DepthAttachment, GL_TEXTURE_2D, DepthID, 0);
		//GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		//if (statues != GL_FRAMEBUFFER_COMPLETE)//某个绑定到attachment上的元素无效
		//{
		//	Log::GetInstance()->logMessage("GLTexture::attachToRenderTarget() : Error");
		//}
	}
}

void GLRenderSystem::SetGraphicsPipelineState(RSGraphicPipelineState& InPipelineState)
{
	if (InPipelineState.RasterizerState.Get() && CachedPipelineState.RasterizerState != InPipelineState.RasterizerState)
	{
		CachedPipelineState.RasterizerState = InPipelineState.RasterizerState;
		GLStaticRasterizerState* RasterizerState = dynamic_cast<GLStaticRasterizerState*>(InPipelineState.RasterizerState.Get());
		glPolygonMode(GL_FRONT_AND_BACK, RasterizerState->FillMode);
		Helper::SetIsEnableState(RasterizerState->IsEnableMSAA, GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		if (RasterizerState->CullMode != GL_CULL_FACE)
		{
			glCullFace(RasterizerState->CullMode);
		}
	}
    if (InPipelineState.DepthAndStencilState.Get() && CachedPipelineState.DepthAndStencilState != InPipelineState.DepthAndStencilState)
    {
		CachedPipelineState.DepthAndStencilState = InPipelineState.DepthAndStencilState;
		GLStaticDepthAndStencilState* DepthAndStencilState = dynamic_cast<GLStaticDepthAndStencilState*>(InPipelineState.DepthAndStencilState.Get());
		Check(DepthAndStencilState);
		Helper::SetIsEnableState(DepthAndStencilState->IsEnableDepthTest, GL_DEPTH_TEST);
		CHECK_GL_ERROR(glDepthMask(DepthAndStencilState->IsEnableDepthWrite));
    }
	if (InPipelineState.BlendState.Get() && CachedPipelineState.BlendState != InPipelineState.BlendState)
	{
		CachedPipelineState.BlendState = InPipelineState.BlendState;
		GLStaticBlendState* BlendState = dynamic_cast<GLStaticBlendState*>(InPipelineState.BlendState.Get());
		Helper::SetIsEnableState(BlendState->IsEnableBlend, GL_BLEND);
		if (BlendState->IsSepatate == GL_TRUE)
		{
			CHECK_GL_ERROR(glBlendEquationSeparate(BlendState->RGBBlendEquation, BlendState->AlphaBlendEquation));
			CHECK_GL_ERROR(glBlendFuncSeparate(BlendState->RGBFactorS, BlendState->RGBFactorD,
				BlendState->AlphaFactorS, BlendState->AlphaFactorD));
		}
		else
		{
			CHECK_GL_ERROR(glBlendEquation(BlendState->RGBBlendEquation));
			CHECK_GL_ERROR(glBlendFunc(BlendState->RGBFactorS, BlendState->RGBFactorD));
		}
	}
	if (InPipelineState.ColorMaskState.Get() && CachedPipelineState.ColorMaskState != InPipelineState.ColorMaskState)
	{
		SetColorMaskState(InPipelineState.ColorMaskState);
	}
	CachedPipelineState.GPUProgramUsage = InPipelineState.GPUProgramUsage;
}


void GLRenderSystem::SetGraphicsPipelineStateImmediately(RSGraphicPipelineState& InPipelineState)
{
	if (InPipelineState.RasterizerState->IsDirty)
	{
		GLStaticRasterizerState* RasterizerState = dynamic_cast<GLStaticRasterizerState*>(InPipelineState.RasterizerState.Get());
		glPolygonMode(GL_FRONT_AND_BACK, RasterizerState->FillMode);
		Helper::SetIsEnableState(RasterizerState->IsEnableMSAA, GL_MULTISAMPLE);
		glEnable(GL_CULL_FACE);
		if (RasterizerState->CullMode != GL_CULL_FACE)
		{
			glCullFace(RasterizerState->CullMode);
		}
		InPipelineState.RasterizerState->IsDirty = false;
	}
	if (InPipelineState.DepthAndStencilState->IsDirty)
	{
		GLStaticDepthAndStencilState* DepthAndStencilState = dynamic_cast<GLStaticDepthAndStencilState*>(InPipelineState.DepthAndStencilState.Get());
		Check(DepthAndStencilState);
		Helper::SetIsEnableState(DepthAndStencilState->IsEnableDepthTest, GL_DEPTH_TEST);
		glDepthMask(DepthAndStencilState->IsEnableDepthWrite);
		InPipelineState.DepthAndStencilState->IsDirty = false;
	}
	/*if (CachedPipelineState.GPUProgram != InPipelineState.GPUProgram)
	{
	CachedPipelineState.GPUProgram = InPipelineState.GPUProgram;
	dynamic_cast<GLSLGpuProgram*>(CachedPipelineState.GPUProgram.Get())->bind();
	}*/
	if (InPipelineState.BlendState->IsDirty)
	{
		/*GLStaticBlendState* BlendState = dynamic_cast<GLStaticBlendState*>(InPipelineState.BlendState.Get());
		Helper::SetIsEnableState(BlendState->IsEnableBlend, GL_BLEND);
		glBlendEquation(BlendState->BlendEquation);
		glBlendFunc(BlendState->FactorS, BlendState->FactorD);
		InPipelineState.BlendState->IsDirty = false;*/
	}
	CachedPipelineState.GPUProgramUsage = InPipelineState.GPUProgramUsage;

	BWHighLevelGpuProgramPtr GPUProgram = InPipelineState.GPUProgramUsage->GetHighLevelGpuProgram();
	GLSLGpuProgram* GLGPUProgram = dynamic_cast<GLSLGpuProgram*>(GPUProgram.Get());
	GLGPUProgram->bind();
	GPUProgram->SetGPUProgramParameters(CachedPipelineState.GPUProgramUsage->GetGpuProgramParameter());
}

void GLRenderSystem::SetShaderTexture(BWHighLevelGpuProgramPtr GPUProgram, BWTexturePtr Texture, SamplerStateHIRef Sampler)
{
	GLTexture* TextureHI = dynamic_cast<GLTexture*>(Texture.Get());
	if (TextureHI)
	{
		int Index = TextureHI->GetIndex();
		GLint ID = TextureHI->GetHIID(); 
		GLenum Type = Helper::GetGLTextureType(TextureHI->GetTextureType());
		Check(Index < MaxActiveTexteureNum);
		CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + Index));
		CHECK_GL_ERROR(glBindTexture(Type, ID));
		
		CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_BASE_LEVEL, 0));
		if (TextureHI->getNumMipmaps() > 0)
		{
			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MAX_LEVEL, TextureHI->getNumMipmaps() - 1));
		}
		// 这里的这些状态可以考虑使用一个使用OpenGL中Sampler Object
		
		{
			GLStaticSamplerState* GLSampler = dynamic_cast<GLStaticSamplerState*>(Sampler.Get());
			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_WRAP_R, GLSampler->RAdd_Mode));
			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_WRAP_S, GLSampler->SAdd_Mode));
			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_WRAP_T, GLSampler->TAdd_Mode));
			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, GLSampler->MinFilter)); 
			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, GLSampler->MagFilter));
		}
	}
	
}


void GLRenderSystem::SetShaderTextureImmediately(const std::vector<RSShaderTexture>& InShaderTexures)
{
	for (int i = 0; i < InShaderTexures.size(); i++)
	{
		GLTexture* TextureHI = dynamic_cast<GLTexture*>(InShaderTexures[i].Texture.Get());
		if (TextureHI)
		{
			int Index = TextureHI->GetIndex();
			GLint ID = TextureHI->GetHIID();
			GLenum Type = Helper::GetGLTextureType(TextureHI->GetTextureType());
			Check(Index < MaxActiveTexteureNum);
			CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0 + Index));
			CHECK_GL_ERROR(glBindTexture(Type, ID));

			CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_BASE_LEVEL, 0));
			if (TextureHI->getNumMipmaps() > 0)
			{
				CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MAX_LEVEL, TextureHI->getNumMipmaps() - 1));
			}
			// 这里的这些状态可以考虑使用一个使用OpenGL中Sampler Object
			if (ShaderTextures[i].Sampler->IsDirty)
			{
				GLStaticSamplerState* GLSampler = dynamic_cast<GLStaticSamplerState*>(ShaderTextures[i].Sampler.Get());
				CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_WRAP_R, GLSampler->RAdd_Mode));
				CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_WRAP_S, GLSampler->SAdd_Mode));
				CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_WRAP_T, GLSampler->TAdd_Mode));
				//CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, GLSampler->Fileter)); 这种使用方式有问题
				CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
				CHECK_GL_ERROR(glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
				GLSampler->IsDirty = false;
			}
		}
	}
	
}

void GLRenderSystem::ClearRenderTarget(unsigned int buffers, const ColourValue &color /*= ColourValue::Black */, float depth /*= 1.0 */, unsigned short stencil /*= 0 */)
{
	unsigned int flag = 0;
	if (buffers & FBT_COLOUR)
	{
		flag |= GL_COLOR_BUFFER_BIT;
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClearColor(color.r, color.g, color.b, color.a);
	}
	if (buffers & FBT_DEPTH)
	{
		flag |= GL_DEPTH_BUFFER_BIT;
		glDepthMask(GL_TRUE);
		glClearDepth(depth);
	}
	if (buffers & FBT_STENCIL)
	{
		flag |= GL_STENCIL_BUFFER_BIT;
		glStencilMask(0xffffffff);
		glClearStencil(stencil);
	}
	// 裁剪测试是否开启
	GLboolean scissorTestEnabled = glIsEnabled(GL_SCISSOR_TEST);
	if (!scissorTestEnabled)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	GLint viewport[4], scissor[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetIntegerv(GL_SCISSOR_BOX, scissor);
	bool scissorboxDifference = viewport[0] != scissor[0] || viewport[1] != scissor[1] || viewport[2] != scissor[2] || viewport[3] != scissor[3];
	if (scissorboxDifference)
	{
		glScissor(viewport[0], viewport[1], viewport[2], viewport[3]);
	}
	glClear(flag);
	if (scissorboxDifference)
	{
		glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
	}
	if (!scissorTestEnabled)
	{
		glDisable(GL_SCISSOR_TEST);
	}
	SetDepthAndStencilState(CachedPipelineState.DepthAndStencilState);
	SetColorMaskState(CachedPipelineState.ColorMaskState);
}

void GLRenderSystem::ReadSurfaceData(BWTexturePtr SourceTexture, int Index, int MipLevel, BWPixelBox& Destination)
{
	//可以看看Unreal中的RHIReadSurfaceFloatData 函数  看看Opengl是如何管理FBO的
	// there is not exist glGetTexSubImage . So we complete this function with pbo
	GLTexture* Texture = dynamic_cast<GLTexture*>(SourceTexture.Get());
	if (Texture)
	{
		GLenum GLTexutreType = Helper::GetGLTextureType(Texture->GetTextureType());
		GLint CurrentBindTexture = 0 ;
		GLint CurrentFrameBuffer = 0;
		GLenum DesFormat = GLPixelUtil::getGLOriginFormat(Destination.mPixelFormat);
		GLenum DesDataType = GLPixelUtil::getGLOriginDataType(Destination.mPixelFormat);
		switch (GLTexutreType)
		{
		case GL_TEXTURE_2D:
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &CurrentBindTexture);
			break;
		case GL_TEXTURE_CUBE_MAP:
			glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP, &CurrentBindTexture);
			break;
		}
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);


		GLuint NewFrameBuffer;
		glGenFramebuffers(1, &NewFrameBuffer);
		CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, NewFrameBuffer));
		GLint TextureID = Texture->GetHIID();
		TextureType RenderTextureType = Texture->GetTextureType();
		GLenum OutputFormat = GLPixelUtil::getGLOriginFormat(Destination.mPixelFormat);
		GLenum OutputDataType = GLPixelUtil::getGLOriginDataType(Destination.mPixelFormat);

		if (RenderTextureType == TEX_TYPE_2D)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TextureID, MipLevel));  //暂时使用2D纹理
		}
		if (RenderTextureType == TEX_TYPE_CUBE_MAP)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + Index, TextureID, MipLevel));
		}
		glBindFramebuffer(GL_READ_FRAMEBUFFER, NewFrameBuffer);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		CHECK_GL_ERROR(glPixelStorei(GL_PACK_ALIGNMENT, 1));
		CHECK_GL_ERROR(glReadPixels(Destination.left ,Destination.bottom, Destination.right, Destination.top, OutputFormat, OutputDataType, Destination.mData));
		glPixelStorei(GL_PACK_ALIGNMENT, 4);
		glDeleteFramebuffers(1, &NewFrameBuffer);

		glBindFramebuffer(GL_FRAMEBUFFER, CurrentFrameBuffer);
		glBindTexture(GLTexutreType, CurrentBindTexture);
	}

}

RasterizerStateHIRef GLRenderSystem::CreateRasterizerStateHI(RasterStateInitializer& Initializer)
{
	GLStaticRasterizerState* RenderState = new GLStaticRasterizerState();
	RenderState->CullMode = Helper::GetCullMode(Initializer.CullMode);
	RenderState->FillMode = Helper::GetFillMode(Initializer.FillMode);
	RenderState->IsEbaleLineAA = Helper::GetBool(Initializer.IsEnableLineAA);
	RenderState->IsEnableMSAA = Helper::GetBool(Initializer.IsEnableMSAA);
	return RenderState;
}

DepthAndStencilStateHIRef GLRenderSystem::CreateDepthAndStencilHI(DepthAndStencilInitializer& Initializer)
{
	GLStaticDepthAndStencilState* RenderState = new GLStaticDepthAndStencilState();
	RenderState->IsEnableDepthTest = Helper::GetBool(Initializer.IsEnableDepthTest);
	RenderState->IsEnableDepthWrite = Helper::GetBool(Initializer.IsEnableDepthWrite);
	return RenderState;
}

SamplerStateHIRef GLRenderSystem::CreateSamplerStateHI(StaticSamplerStateInitializer& Initializer)
{
	GLStaticSamplerState* RenderState = new GLStaticSamplerState();
	RenderState->MinFilter = Helper::GetGLFilterOptions(Initializer.MinFilter , Initializer.MipFilter);
	RenderState->MagFilter = Helper::GetGLFilterOptions(Initializer.MagFilter);
	RenderState->RAdd_Mode = Helper::GetGLTextureSwap(Initializer.RAdd_Mode);
	RenderState->SAdd_Mode = Helper::GetGLTextureSwap(Initializer.SAdd_Mode);
	RenderState->TAdd_Mode = Helper::GetGLTextureSwap(Initializer.TAdd_Mode);
	RenderState->MipBias = Initializer.MipBias;
	return RenderState;
}


BlendStateHIRef GLRenderSystem::CreateBlendStateHI(StaticBlendStateInitializer& Initializer)
{
	GLStaticBlendState* RenderState = new GLStaticBlendState();
	RenderState->IsEnableBlend = Helper::GetBool(Initializer.IsEnableBlend);
	RenderState->RGBBlendEquation = Helper::GetBlendEquation(Initializer.RGBBlendEquation);
	RenderState->RGBFactorS = Helper::GetBlendFactor(Initializer.RGBFactorS);
	RenderState->RGBFactorD = Helper::GetBlendFactor(Initializer.RGBFactorD);

	RenderState->IsSepatate = Helper::GetBool(Initializer.IsSepatate);
	RenderState->AlphaBlendEquation = Helper::GetBlendEquation(Initializer.AlphaBlendEquation);
	RenderState->AlphaFactorS = Helper::GetBlendFactor(Initializer.AlphaFactorS);
	RenderState->AlphaFactorD = Helper::GetBlendFactor(Initializer.AlphaFactorD);

	return RenderState;
}


ColorMaskStateHIRef GLRenderSystem::CreateColorMaskState(StaticColorMaskStateInitializer& Initializer)
{
	GLStaticColorMaskState* ColorMask = new GLStaticColorMaskState();
	ColorMask->Red = Helper::GetBool(Initializer.Red);
	ColorMask->Green = Helper::GetBool(Initializer.Green);
	ColorMask->Blue = Helper::GetBool(Initializer.Blue);
	ColorMask->Alpha = Helper::GetBool(Initializer.Alpha);
	return ColorMask;
}

void GLRenderSystem::SetColorMaskState(ColorMaskStateHIRef ColorMask)
{
	CachedPipelineState.ColorMaskState = ColorMask;
	GLStaticColorMaskState* ColorMaskState = dynamic_cast<GLStaticColorMaskState*>(ColorMask.Get());
	glColorMask(ColorMaskState->Red, ColorMaskState->Green, ColorMaskState->Blue, ColorMaskState->Alpha);
}

void GLRenderSystem::SetDepthAndStencilState(DepthAndStencilStateHIRef InDepthAndStencilState)
{
	CachedPipelineState.DepthAndStencilState = InDepthAndStencilState;
	GLStaticDepthAndStencilState* DepthAndStencilState = dynamic_cast<GLStaticDepthAndStencilState*>(InDepthAndStencilState.Get());
	Check(DepthAndStencilState);
	Helper::SetIsEnableState(DepthAndStencilState->IsEnableDepthTest, GL_DEPTH_TEST);
	CHECK_GL_ERROR(glDepthMask(DepthAndStencilState->IsEnableDepthWrite));
}

void GLRenderSystem::ClearTextureResource()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLRenderSystem::BeginDeferLight()
{
	//~!~!glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GLRenderSystem::PointLightPass()
{
	return;
	//点光源光照
	BWGpuProgramParametersPtr GpuProgramParameter = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter();
	/*BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->SetGPUAutoParameter(GpuProgramParameter);*/

	BWMatrix4 ViewMatrix, ProjectMatrix;
	if (GpuProgramParameter.Get() != NULL)
	{
		const BWGpuProgramParameters::AutoConstantList &autoConstantList = GpuProgramParameter->GetAutoConstantList();
		BWGpuProgramParameters::AutoConstantList::const_iterator itor = autoConstantList.begin();
		while (itor != autoConstantList.end())
		{
			switch (itor->paramType)
			{
			case BWGpuProgramParameters::ACT_VIEW_MATRIX:
			{
				//这里有些问题 因为渲染流程的起始点是renderTarget到camera  而在windrendertarget中是没有camera的 
				//这里暂时让wind使用的摄像机和上一次的摄像机保持一致
				BWMatrix4 tmp;
				tmp = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->getViewMatrix();
				GpuProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_WORLD_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY;
				GpuProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_PROJECTION_MATRIX:
			{
				BWMatrix4 tmp = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->getProjectionMatrix();
				GpuProgramParameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			default:
				break;
			}
			itor++;
		}
	}

	
	mPointLightDefferLightingGLSLProgram->bind();


	//GpuNamedConstantsPtr namedConstant = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->getNamedConstants();
	//if (namedConstant.IsNull())
	//{
	//	namedConstant = mPointLightDefferLightingGLSLProgram->GetDefaultParameters()->getNamedConstants();
	//}
	////设置各种自动unifomr参数
	//GpuConstantDefinitionMap::iterator itor = namedConstant->map.begin();
	//while (itor != namedConstant->map.end())
	//{
	//	GpuConstantDefinition *def = &(itor->second);
	//	int size = def->arraySize * itor->second.getElementSize(def->constType, false);
	//	void *data = NULL;
	//	int a = def->physicalIndex;
	//	if (def->isFloat())
	//	{
	//		data = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->GetFloatPointer(def->physicalIndex);
	//	}
	//	else
	//	{
	//		data = mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->GetIntPointer(def->physicalIndex);
	//	}
	//	if (itor->second.getElementSize(def->constType, false) > 4)
	//	{
	//		// 说明是矩阵需要转置
	//		float tmpdata[16];
	//		float *dataTest = (float*)data;
	//		for (int i = 0; i < 16; i++)
	//		{
	//			tmpdata[i] = dataTest[i];
	//		}
	//		GLRenderSystem::GLtranspose(def->constType, tmpdata);
	//		mPointLightDefferLightingGLSLProgram->SetParameter(itor->first, tmpdata);
	//	}
	//	else
	//	{
	//		mPointLightDefferLightingGLSLProgram->SetParameter(itor->first, data);
	//	}
	//	itor++;
	//}

	/*
	 这种使用方式有错！！！！！！！！！！！！！！ 会影响其他buffer
	CHECK_GL_ERROR(glBindBuffer(GL_ARRAY_BUFFER, mPointLightDefferLightingBufferID));
	NameLocation& tmpNameLoaction = mPointLightDefferLightingGLSLProgram->mNameLoaction;
	NameLocation::iterator namedLoactionItor = tmpNameLoaction.find(mBufferElementToString[VES_POSITION]);
	if (namedLoactionItor != tmpNameLoaction.end())
	{
		GLint location = namedLoactionItor->second;
		if (location != -1)
		{
			CHECK_GL_ERROR(glEnableVertexAttribArray(location));
			CHECK_GL_ERROR(glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 32, 0));
		}
	}
	CHECK_GL_ERROR(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mPointLightDefferLightingIndexBufferID));
*/

	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, GLABufferTexture->GetHIID()));

	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE1));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, GLNormalTexture->GetHIID()));

	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE2));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, GLPositionTexture->GetHIID()));

	// 这里设置光源的独有参数

	PointLightMap& PointLights = BWRoot::GetInstance()->getSceneManager()->GetActivePointLights();
	PointLightMap::iterator PointLight = PointLights.begin();
	while (PointLight != PointLights.end())
	{
		BWPointLight* Light = PointLight->second;
		BWSceneNode* SceneNode = Light->getParentSceneNode();
		const BWVector3& WorldPosition = Light->GetWorldPosition();
		float Radiuse = Light->getSpherePointRadius();
		BWMatrix4 Matrix;
		SceneNode->getWorldTransforms(&Matrix);
		GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, &Matrix);
		GLfloat mat[16];
		for (int i = 0; i < 16; i++)
			mat[i] = Matrix.M[i];
		float ScreenSize[2]; ScreenSize[0] = 1024.0f; ScreenSize[1] = 768.0f;
		float Pos[3]; Pos[0] = WorldPosition.x; Pos[1] = WorldPosition.y; Pos[2] = WorldPosition.z;
		mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("RadiusScale",&Radiuse, 1, 1);
		mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("PointLightPosition", Pos, 3, 1);
		mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("gScreenSize", ScreenSize, 2, 1);
		mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter()->SetNamedConstant("tmpModelMatrix", mat, 16, 1);
		mPointLightDefferLightingGLSLProgram->SetGPUProgramParameters(mPointLightDefferLightingGpuPrgramUsage->GetGpuProgramParameter());
		//生成点光源的stencil  其实使用的就是渲染光源的流程  就是渲染的时候将renderbuffer改为none 以及设置一些状态而已
		CHECK_GL_ERROR(glDrawBuffer(GL_NONE));
		glClearStencil(0);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDepthMask(GL_FALSE);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 0, 0);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
		//glDrawElements(GL_TRIANGLES, SphereVertexNum, GL_UNSIGNED_SHORT, NULL);
		RenderRenderOperationWithPointLight(SphereMeshRenderOperation);
	    //开始延迟光照
		CHECK_GL_ERROR(glDrawBuffer(FinalRenderResult->getAttachment()));
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_DEPTH_TEST);
		glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);// 为了避免当摄像机在光源内部的时候出现无法渲染的情况
		RenderRenderOperationWithPointLight(SphereMeshRenderOperation);
		PointLight++;
	}

	CHECK_GL_ERROR(glActiveTexture(GL_TEXTURE0)); // 一定要恢复到这里
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	return;
}

void GLRenderSystem::SetRenderShadowMapRenderTarget()
{
	//CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, GLShadowMapRenderTarget->getID()));
}

void GLRenderSystem::SetLightPreparForRenderShadow(BWDirectionalLight * DirectionalLight)
{
	//GLShadowMapRenderTarget->addDepthRenderBuffer(DirectionalLight->GetShadowMap());
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, GLShadowMapRenderTarget->getID()));
	CHECK_GL_ERROR(glDrawBuffer(GL_NONE));
	GLbitfield clearTag = GL_DEPTH_BUFFER_BIT;
	CHECK_GL_ERROR(glClear(clearTag));  // 这里一定要清理 否则后面的framebuffer会把数据写入进入 不太清楚为啥

	DirectionalLightShadowMapGLSLProgram->bind();

	BWGpuProgramParametersPtr parameter = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter();
	BWMatrix4 ViewMatrix, ProjectMatrix;
	if (parameter.Get() != NULL)
	{
		const BWGpuProgramParameters::AutoConstantList &autoConstantList = parameter->GetAutoConstantList();
		BWGpuProgramParameters::AutoConstantList::const_iterator itor = autoConstantList.begin();
		while (itor != autoConstantList.end())
		{
			switch (itor->paramType)
			{
			case BWGpuProgramParameters::ACT_VIEW_MATRIX:
			{
				//这里有些问题 因为渲染流程的起始点是renderTarget到camera  而在windrendertarget中是没有camera的 
				//这里暂时让wind使用的摄像机和上一次的摄像机保持一致
				BWMatrix4 tmp;
				tmp = DirectionalLight->getViewMatrix();
				ViewMatrix = tmp;
				//GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, &ViewMatrix);
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_WORLD_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY;
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_PROJECTION_MATRIX:
			{

				//BWMatrix4 tmp = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->getProjectionMatrix();
				BWMatrix4 tmp = DirectionalLight->getProjectionMatrix();
				//tmp = DirectionalLight->GetShadowMapProjectedInfor(0).ProjectMatrix;
				ProjectMatrix = tmp;
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				
				//GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, &ProjectMatrix);
				break;
			}
			default:
				break;
			}
			itor++;
		}
	}


	DirectionalLightShadowMapGLSLProgram->bind();


	GpuNamedConstantsPtr namedConstant = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter()->getNamedConstants();
	if (namedConstant.IsNull())
	{
		namedConstant = DirectionalLightShadowMapGLSLProgram->GetDefaultParameters()->getNamedConstants();
	}
	//设置各种自动unifomr参数
	GpuConstantDefinitionMap::iterator itor = namedConstant->map.begin();
	while (itor != namedConstant->map.end())
	{
		GpuConstantDefinition *def = &(itor->second);
		int size = def->arraySize * itor->second.getElementSize(def->constType, false);
		void *data = NULL;
		int a = def->physicalIndex;
		if (def->isFloat())
		{
			data = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter()->GetFloatPointer(def->physicalIndex);
		}
		else
		{
			data = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter()->GetIntPointer(def->physicalIndex);
		}
		//if (itor->second.getElementSize(def->constType, false) > 4)
		//{
		//	//// 说明是矩阵需要转置
		//	float tmpdata[16];
		//	float *dataTest = (float*)data;
		//	for (int i = 0; i < 16; i++)
		//	{
		//		tmpdata[i] = dataTest[i];
		//	}
		//	GLRenderSystem::GLtranspose(def->constType, tmpdata);
		//	DirectionalLightShadowMapGLSLProgram->SetParameter(itor->first, tmpdata);
		//}
		//else
		//{
		//	DirectionalLightShadowMapGLSLProgram->SetParameter(itor->first, data);
		//}
		DirectionalLightShadowMapGLSLProgram->SetParameter(itor->first, data ,def->constType);
		itor++;
	}
}
void GLRenderSystem::RemoveLightFromRenderShadow(BWDirectionalLight* Light)
{
	//GLShadowMapRenderTarget->removeDpethRenderBuffer(Light->GetShadowMap()->getName());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void GLRenderSystem::RenderRenderableShadow(BWRenderable * Renderable)
{
	BWRenderOperation RenderOperation;
	Renderable->getRenderOperation(RenderOperation);
	if (!RenderOperation.useIndexes)
	{
		return;
	}
	BWMatrix4 WorldTransform;
	GLint TmpModelMatrixlocation = glGetUniformLocation(DirectionalLightShadowMapGLSLProgram->getID(), "tmpModelMatrix");
	Renderable->getWorldTransforms(&WorldTransform);
	GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, &WorldTransform);
	GLfloat mat[16];
	for (int i = 0; i < 16; i++)
		mat[i] = WorldTransform.M[i];
	CHECK_GL_ERROR(glUniformMatrix4fv(TmpModelMatrixlocation, 1, false, mat));
	DirectionalLightShadowMapGLSLProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);

	// 这里设置各种状态 只用进行深度渲染 其他的都不用
	GLint primType;
	bool useAdjacenty = false;
	GLenum indexType = (RenderOperation.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	switch (RenderOperation.operationType)
	{
	case  BWRenderOperation::OT_POINT_LIST:
		primType = GL_POINT;
		break;
	case BWRenderOperation::OT_LINE_LIST:
		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
		break;
	case  BWRenderOperation::OT_LINE_STRIP:
		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
		break;
	case  BWRenderOperation::OT_TRIANGLE_LIST:
		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		break;
	case  BWRenderOperation::OT_TRIANGLE_STRIP:
		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
	case BWRenderOperation::OT_TRIGANLE_FAN:
		primType = GL_TRIANGLE_FAN;
	default:
		break;
	}
	glDrawElements(primType, RenderOperation.indexData->mIndexCount, indexType, NULL);
}
void GLRenderSystem::RenderRenderOperation(BWRenderOperation & RenderOperation ,BWMatrix4& ModelMatrix)
{
	if (!RenderOperation.useIndexes)
	{
		return;
	}
	DirectLightGLSLProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);

	// 这里设置各种状态 只用进行深度渲染 其他的都不用
	GLint primType;
	bool useAdjacenty = false;
	GLenum indexType = (RenderOperation.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	switch (RenderOperation.operationType)
	{
	case  BWRenderOperation::OT_POINT_LIST:
		primType = GL_POINT;
		break;
	case BWRenderOperation::OT_LINE_LIST:
		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
		break;
	case  BWRenderOperation::OT_LINE_STRIP:
		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
		break;
	case  BWRenderOperation::OT_TRIANGLE_LIST:
		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		break;
	case  BWRenderOperation::OT_TRIANGLE_STRIP:
		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
	case BWRenderOperation::OT_TRIGANLE_FAN:
		primType = GL_TRIANGLE_FAN;
	default:
		break;
	}
	glDrawElements(primType, RenderOperation.indexData->mIndexCount, indexType, NULL);
}

void GLRenderSystem::RenderOperation(BWRenderOperation & RenderOperation, GLSLGpuProgram * GPUProgram)
{
	if (!RenderOperation.useIndexes)
	{
		return;
	}
	GPUProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);
	// 这里设置各种状态 只用进行深度渲染 其他的都不用
	GLint primType;
	bool useAdjacenty = false;
	GLenum indexType = (RenderOperation.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	switch (RenderOperation.operationType)
	{
	case  BWRenderOperation::OT_POINT_LIST:
		primType = GL_POINT;
		break;
	case BWRenderOperation::OT_LINE_LIST:
		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
		break;
	case  BWRenderOperation::OT_LINE_STRIP:
		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
		break;
	case  BWRenderOperation::OT_TRIANGLE_LIST:
		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		break;
	case  BWRenderOperation::OT_TRIANGLE_STRIP:
		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
	case BWRenderOperation::OT_TRIGANLE_FAN:
		primType = GL_TRIANGLE_FAN;
	default:
		break;
	}
	glDrawElements(primType, RenderOperation.indexData->mIndexCount, indexType, NULL);
}


void GLRenderSystem::RenderOperation(BWRenderOperation & RenderOperation, BWHighLevelGpuProgramPtr GPUProgram)
{
	if (!RenderOperation.useIndexes)
	{
		return;
	}
	//SetGraphicsPipelineStateImmediately(CachedPipelineState);
	//SetRenderTargetImmediately(CacheRenderTarget);
	//SetShaderTextureImmediately(ShaderTextures);
	if (GPUProgram.Get())
	{
		GLSLGpuProgram* GLGPUProgram = dynamic_cast<GLSLGpuProgram*>(GPUProgram.Get());
		GLGPUProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);
	}
	else
	{
		BWHighLevelGpuProgramPtr GPUProgram = CachedPipelineState.GPUProgramUsage->GetHighLevelGpuProgram();
		GLSLGpuProgram* GLGPUProgram = dynamic_cast<GLSLGpuProgram*>(GPUProgram.Get());
		GLGPUProgram->bind();
		GPUProgram->SetGPUProgramParameters(CachedPipelineState.GPUProgramUsage->GetGpuProgramParameter());
		GLGPUProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);
	}
	
	// 这里设置各种状态 只用进行深度渲染 其他的都不用
	GLint primType;
	bool useAdjacenty = false;
	GLenum indexType = (RenderOperation.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	switch (RenderOperation.operationType)
	{
	case  BWRenderOperation::OT_POINT_LIST:
		primType = GL_POINT;
		break;
	case BWRenderOperation::OT_LINE_LIST:
		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
		break;
	case  BWRenderOperation::OT_LINE_STRIP:
		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
		break;
	case  BWRenderOperation::OT_TRIANGLE_LIST:
		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		break;
	case  BWRenderOperation::OT_TRIANGLE_STRIP:
		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
	case BWRenderOperation::OT_TRIGANLE_FAN:
		primType = GL_TRIANGLE_FAN;
	default:
		break;
	}
	glDrawElements(primType, RenderOperation.indexData->mIndexCount, indexType, NULL);
}


void GLRenderSystem::CopyTextureToTexture(BWTexturePtr SourceTexture, int SourceIndex, int SourceMipmipLevel, BWTexturePtr DestinationTexture, int DestinationIndex, int DestinationMipmapLevel)
{

	// This Function Can Break Current FrameBufferBind
	GLuint SourceFrameBuffer, DestinationFrameBuffer;
	GLTexture* GLSourceTexture = dynamic_cast<GLTexture*>(SourceTexture.Get());
	GLTexture* GLDestinationTexture = dynamic_cast<GLTexture*>(DestinationTexture.Get());
	GLint DestWidth = DestinationTexture->getWidth() * std::pow(0.5, DestinationMipmapLevel);
	GLint DestHieght = DestinationTexture->getHeight() * std::pow(0.5, DestinationMipmapLevel);
	GLint SourceWidth = SourceTexture->getWidth()* std::pow(0.5, SourceMipmipLevel);
	GLint SourceHight = SourceTexture->getHeight() * std::pow(0.5, SourceMipmipLevel);
	if (DestHieght == 0) DestHieght = 1;
	if (DestWidth == 0) DestWidth = 1;
	if (SourceHight == 0) SourceHight = 1;
	if (SourceWidth == 0) SourceWidth = 1;

	GLint DTextureID = GLDestinationTexture->GetHIID();
	TextureType DRenderTextureType = GLDestinationTexture->GetTextureType();

	glGenFramebuffers(1, &SourceFrameBuffer);
	glGenFramebuffers(1, &DestinationFrameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, SourceFrameBuffer);
	GLint STextureID = GLSourceTexture->GetHIID();
	TextureType SRenderTextureType = GLSourceTexture->GetTextureType();
	if (SRenderTextureType == TEX_TYPE_2D)
	{
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, STextureID, SourceMipmipLevel));  //暂时使用2D纹理
	}
	if (SRenderTextureType == TEX_TYPE_CUBE_MAP)
	{
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + SourceIndex, STextureID, SourceMipmipLevel));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, DestinationFrameBuffer);
	if (DRenderTextureType == TEX_TYPE_2D)
	{
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DTextureID, DestinationMipmapLevel));  //暂时使用2D纹理
	}
	if (DRenderTextureType == TEX_TYPE_CUBE_MAP)
	{
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + DestinationMipmapLevel, DTextureID, DestinationMipmapLevel));
	}
	glDrawBuffer(GL_COLOR_ATTACHMENT0);    
	glClearColor(1.0, 1.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, SourceFrameBuffer);
	CHECK_GL_ERROR(glReadBuffer(GL_COLOR_ATTACHMENT0));

	CHECK_GL_ERROR(glBlitFramebuffer(0, 0, SourceWidth, SourceHight,
		0, 0, DestWidth, DestHieght, GL_COLOR_BUFFER_BIT, GL_LINEAR));

	glDeleteFramebuffers(1, &SourceFrameBuffer);
	glDeleteFramebuffers(1, &DestinationFrameBuffer);

}


void GLRenderSystem::CopyTextureToScreen(BWTexturePtr SourceTexture, int SourceIndex, int SourceMipmipLevel)
{
	GLint CurrentFrameBuffer;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &CurrentFrameBuffer);


	GLuint SourceFrameBuffer;
	GLint Width = SourceTexture->getWidth();
	GLint Hieght = SourceTexture->getHeight();
	GLTexture* GLSourceTexture = dynamic_cast<GLTexture*>(SourceTexture.Get());
	glGenFramebuffers(1, &SourceFrameBuffer);
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, SourceFrameBuffer));
	GLint STextureID = GLSourceTexture->GetHIID();
	TextureType SRenderTextureType = GLSourceTexture->GetTextureType();
	if (SRenderTextureType == TEX_TYPE_2D)
	{
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, STextureID, SourceMipmipLevel));  //暂时使用2D纹理
	}
	if (SRenderTextureType == TEX_TYPE_CUBE_MAP)
	{
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + SourceIndex, STextureID, SourceMipmipLevel));
	}
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(1.0, 1.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	CHECK_GL_ERROR(glBindFramebuffer(GL_READ_FRAMEBUFFER, SourceFrameBuffer));
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	CHECK_GL_ERROR(glBlitFramebuffer(0, 0, Width, Hieght,
		0, 0, Width, Hieght, GL_COLOR_BUFFER_BIT, GL_LINEAR));
	glDeleteFramebuffers(1, &SourceFrameBuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, CurrentFrameBuffer);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GLRenderSystem::RenderRenderOperationWithPointLight(BWRenderOperation &RenderOperation)
{
	if (!RenderOperation.useIndexes)
	{
		return;
	}
	mPointLightDefferLightingGLSLProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);

	// 这里设置各种状态 只用进行深度渲染 其他的都不用
	GLint primType;
	bool useAdjacenty = false;
	GLenum indexType = (RenderOperation.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
	switch (RenderOperation.operationType)
	{
	case  BWRenderOperation::OT_POINT_LIST:
		primType = GL_POINT;
		break;
	case BWRenderOperation::OT_LINE_LIST:
		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
		break;
	case  BWRenderOperation::OT_LINE_STRIP:
		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
		break;
	case  BWRenderOperation::OT_TRIANGLE_LIST:
		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
		break;
	case  BWRenderOperation::OT_TRIANGLE_STRIP:
		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
	case BWRenderOperation::OT_TRIGANLE_FAN:
		primType = GL_TRIANGLE_FAN;
	default:
		break;
	}
	glDrawElements(primType, RenderOperation.indexData->mIndexCount, indexType, NULL);
}
//void GLRenderSystem::RenderRenderOperationWithSkyBox(BWRenderOperation &RenderOperation)
//{
//	if (!RenderOperation.useIndexes)
//	{
//		return;
//	}
//	class GLSLGpuProgram* mSkyBoxProgram = dynamic_cast<GLSLGpuProgram*>(mSkyBoxGpuPrgramUsage->GetGpuProgram().Get());
//	mSkyBoxProgram->bindingBuffer(RenderOperation.indexData, RenderOperation.vertexData);
//
//	// 这里设置各种状态 只用进行深度渲染 其他的都不用
//	GLint primType;
//	bool useAdjacenty = false;
//	GLenum indexType = (RenderOperation.indexData->mIndexBuffer->getIndexType() == BWHardwareIndexBuffer::IT_16BIT) ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
//	switch (RenderOperation.operationType)
//	{
//	case  BWRenderOperation::OT_POINT_LIST:
//		primType = GL_POINT;
//		break;
//	case BWRenderOperation::OT_LINE_LIST:
//		primType = useAdjacenty ? GL_LINES_ADJACENCY : GL_LINES;
//		break;
//	case  BWRenderOperation::OT_LINE_STRIP:
//		primType = useAdjacenty ? GL_LINE_STRIP_ADJACENCY : GL_LINE_STRIP;
//		break;
//	case  BWRenderOperation::OT_TRIANGLE_LIST:
//		primType = useAdjacenty ? GL_TRIANGLES_ADJACENCY : GL_TRIANGLES;
//		break;
//	case  BWRenderOperation::OT_TRIANGLE_STRIP:
//		primType = useAdjacenty ? GL_TRIANGLE_STRIP_ADJACENCY : GL_TRIANGLE_STRIP;
//	case BWRenderOperation::OT_TRIGANLE_FAN:
//		primType = GL_TRIANGLE_FAN;
//	default:
//		break;
//	}
//	glDrawElements(primType, RenderOperation.indexData->mIndexCount, indexType, NULL);
//}
void GLRenderSystem::FinishLightsShadowMaps()
{

}


void GLRenderSystem::SetProjectedShadowInfoForRenderShadow(ShadowMapProjectInfo& ProjectInfo)
{
	// 绑定方式上有不同
	GLShadowMapRenderTarget->addDepthRenderBuffer(ProjectInfo.ShadowTexture);
	CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, GLShadowMapRenderTarget->getID()));
	CHECK_GL_ERROR(glDrawBuffer(GL_NONE));
	GLbitfield clearTag = GL_DEPTH_BUFFER_BIT;
	CHECK_GL_ERROR(glClear(clearTag));  // 这里一定要清理 否则后面的framebuffer会把数据写入进入 不太清楚为啥

	DirectionalLightShadowMapGLSLProgram->bind();

	BWGpuProgramParametersPtr parameter = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter();
	BWMatrix4 ViewMatrix, ProjectMatrix;
	if (parameter.Get() != NULL)
	{
		const BWGpuProgramParameters::AutoConstantList &autoConstantList = parameter->GetAutoConstantList();
		BWGpuProgramParameters::AutoConstantList::const_iterator itor = autoConstantList.begin();
		while (itor != autoConstantList.end())
		{
			switch (itor->paramType)
			{
			case BWGpuProgramParameters::ACT_VIEW_MATRIX:
			{
				//这里有些问题 因为渲染流程的起始点是renderTarget到camera  而在windrendertarget中是没有camera的 
				//这里暂时让wind使用的摄像机和上一次的摄像机保持一致
				BWMatrix4 tmp;
				tmp = ProjectInfo.ViewMatrix;
				ViewMatrix = tmp;
				//GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, &ViewMatrix);
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_WORLD_MATRIX:
			{
				BWMatrix4 tmp = BWMatrix4::IDENTITY;
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				break;
			}
			case BWGpuProgramParameters::ACT_PROJECTION_MATRIX:
			{

				//BWMatrix4 tmp = BWRoot::GetInstance()->getSceneManager()->getAutoParamDataSource()->getProjectionMatrix();
				BWMatrix4 tmp = ProjectInfo.ProjectMatrix;
				parameter->WriteRawConstants(itor->physicalIndex, tmp.M, 16);
				ProjectMatrix = tmp;
				//GLRenderSystem::GLtranspose(GCT_MATRIX_4X4, &ProjectMatrix);
				break;
			}
			default:
				break;
			}
			itor++;
		}
	}


	DirectionalLightShadowMapGLSLProgram->bind();


	GpuNamedConstantsPtr namedConstant = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter()->getNamedConstants();
	if (namedConstant.IsNull())
	{
		namedConstant = DirectionalLightShadowMapGLSLProgram->GetDefaultParameters()->getNamedConstants();
	}
	//设置各种自动unifomr参数
	GpuConstantDefinitionMap::iterator itor = namedConstant->map.begin();
	while (itor != namedConstant->map.end())
	{
		GpuConstantDefinition *def = &(itor->second);
		int size = def->arraySize * itor->second.getElementSize(def->constType, false);
		void *data = NULL;
		int a = def->physicalIndex;
		if (def->isFloat())
		{
			data = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter()->GetFloatPointer(def->physicalIndex);
		}
		else
		{
			data = DirectionalLightShadowMapGpuPrgramUsage->GetGpuProgramParameter()->GetIntPointer(def->physicalIndex);
		}
		DirectionalLightShadowMapGLSLProgram->SetParameter(itor->first, data, def->constType);
		itor++;
	}
}
void GLRenderSystem::RemoveProjectedShadowInfoFromRenderShadow(ShadowMapProjectInfo& ProjectInfo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GLShadowMapRenderTarget->removeDpethRenderBuffer(ProjectInfo.ShadowTexture->getName());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}