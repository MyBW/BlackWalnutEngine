#ifndef COMMON_H_
#define COMMON_H_

#include<map>
#include "BWLog.h"
typedef std::map<std::string, std::string> AliasTextureNamePairList;
const std::string DEFAULT_RESOURCE_GROUP = "DEFAULT_RESOURCE_GROUP";
#define  BWLOG(Msg) Log::GetInstance()->logMessage(Msg)
#define  Check(Value) assert(Value) 
enum CompareFunction
{
	CMPF_ALWAYS_FAIL,
	CMPF_ALWAYS_PASS,
	CMPF_LESS ,
	CMPF_LESS_EQUAL, //这里要根据不同的API去判断 例如OpenGL 和 Directx 是不同 可以看看Unreal
	CMPF_EQUAL,
	CMPF_NOT_EQUAL,
	CMPF_GREATER_EQUAL,
	CMPF_GREATER
};


typedef int TrackVertexColourType;
enum TrackVertexColourEnum
{
	TVC_NONE = 0x0 ,
	TVC_AMBIENT  = 0X1,
	TVC_DIFFUESE = 0x2 ,
	TVC_SPECULAR = 0x4,
	TVC_EMISSIVE = 0x8
};

enum CullingMode
{
	CULL_NONE = 1 ,
	CULL_CLOCKWISE = 2,
	CULL_ANTICLOCKWISE = 3
};

enum ManualCullingMode
{
	MANUAL_CULL_NONE =1 ,
	MANUAL_CULL_BACK = 2 ,
	MANUAL_CULL_FRONT = 3
};

enum ShadeOptions
{
  SO_FLAT ,
  SO_GOURAUD,
  SO_PHONG
};
enum PolygonMode
{
	PM_SOLID ,
	PM_POINTS,
	PM_WIREFRAME
};
enum FogMode
{
	FOG_NONE,
	FOG_LINEAR,
	FOG_EXP,
	FOG_EXP2
};

enum StencilOperation
{
	SOP_KEEP,
	SOP_ZERO,
	SOP_REPLACE,
	SOP_INCREMENT,
	SOP_DECREMENT,
	SOP_INCREMENT_WRAP,
	SOP_DECREMENT_WRAP,
	SOP_INVERT
};

enum WaveformType
{
	/// Standard sine wave which smoothly changes from low to high and back again.
	WFT_SINE,
	/// An angular wave with a constant increase / decrease speed with pointed peaks.
	WFT_TRIANGLE,
	/// Half of the time is spent at the min, half at the max with instant transition between.
	WFT_SQUARE,
	/// Gradual steady increase from min to max over the period with an instant return to min at the end.
	WFT_SAWTOOTH,
	/// Gradual steady decrease from max to min over the period, with an instant return to max at the end.
	WFT_INVERSE_SAWTOOTH,
	/// Pulse Width Modulation. Works like WFT_SQUARE, except the high to low transition is controlled by duty cycle. 
	/// With a duty cycle of 50% (0.5) will give the same output as WFT_SQUARE.
	WFT_PWM
};

enum TextureFilterOptions
{
	/// Equal to: min=FO_POINT, mag=FO_POINT, mip=FO_NONE
	TFO_NONE,
	/// Equal to: min=FO_LINEAR, mag=FO_LINEAR, mip=FO_POINT
	TFO_BILINEAR,
	/// Equal to: min=FO_LINEAR, mag=FO_LINEAR, mip=FO_LINEAR
	TFO_TRILINEAR,
	/// Equal to: min=FO_ANISOTROPIC, max=FO_ANISOTROPIC, mip=FO_LINEAR
	TFO_ANISOTROPIC
};

enum FilterType
{
	/// The filter used when shrinking a texture
	FT_MIN,
	/// The filter used when magnifying a texture
	FT_MAG,
	/// The filter used when determining the mipmap
	FT_MIP
};

enum FilterOptions
{
	FO_NONE,
	FO_BILINEAR,
	FO_TRILINEAR,
	FO_ANISOTROPIC,
	FO_POINT,
	FO_LINEAR
};
enum SamplerAddressMode
{
	SAM_WRAP,
	SAM_CLAMP,
	SAM_MIRROR,
	SAM_BORDER
};
enum ClipResult
{
	CLIPPED_NONE = 0 ,
	CLIPPED_SOME = 1 ,
	CLIPPED_ALL = 2
};
enum FrameBufferType
{
	FBT_COLOUR = 0x1 ,
	FBT_DEPTH  = 0x2 ,
	FBT_STENCIL = 0x4
};

enum VertexAnimationType
{
	VAT_NONE,
	VAT_MORPH,
	VAT_POSE
};

typedef std::map<std::string, std::string> NameValuePairList;










#define  OGRE_MAX_SIMULTANEOUS_LIGHTS 8
#define  OGRE_MAX_TEXTURE_LAYERS   16
#define OGRE_MAX_SIMULTANEOUS_LIGHTS 8
#define  OGRE_MAX_MULTIPLE_RENDER_TARGETS 16

// 次序越小 渲染优先级越高
#define  OGRE_NUM_RENDERTARGET_GROUPS 10
#define  OGRE_WINDOW_RENDERTARGET_GROUPS 9
#define  OGRE_NORMAL_RENDERTARGET_GROUPS 0

#define MAX_CAST_SHADOW_MAP_NUM 3

#define FORCEINLINE _forceinline
#endif