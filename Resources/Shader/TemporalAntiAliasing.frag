#version 430 core
in vec2 textureCoord ;
uniform sampler2D BBuffer;
uniform sampler2D VelocityRT ;
uniform sampler2D HistoryRT;
uniform sampler2D FinalRenderResult;
uniform int Width ;
uniform int Height ;
uniform float InExposureScale; 
uniform float PlusWeights[5] ;
layout(location = 0) out vec4 FinalAAResult;
layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreModelMatix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
};
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};

vec3 RGBToYCoCg( vec3 RGB )
{
	float Y = dot( RGB, vec3( 1, 2, 1 ) );
	float Co = dot( RGB, vec3( 2, 0, -2 ) );
	float Cg = dot( RGB, vec3( -1, 2, -1 ) );

	vec3 YCoCg = vec3( Y, Co, Cg );
	return YCoCg;
}

vec3 YCoCgToRGB( vec3 YCoCg )
{
	float Y = YCoCg.x * 0.25;
	float Co = YCoCg.y * 0.25;
	float Cg = YCoCg.z * 0.25;

	float R = Y + Co - Cg;
	float G = Y + Cg;
	float B = Y - Co - Cg;

	vec3 RGB = vec3( R, G, B );
	return RGB;
}

void WeightTrackedAlphaClamping(inout float NeighborAlphaMin , inout float NeighborMinWidget,
inout float NeighborAlphaMax , inout float NeighborMaxWidget,
in float NeighborAlpha, in float NeighborWidget)
{
  if(NeighborAlphaMin > NeighborAlpha)
  {
	NeighborAlphaMin = NeighborAlpha ;
	NeighborMinWidget = NeighborWidget ;
  }  
  if(NeighborAlphaMax < NeighborAlpha)
  {
	NeighborAlphaMax = NeighborAlpha ;
	NeighborMaxWidget = NeighborWidget ;
  }
}

vec4 FromScreenToWorld(mat4 InViewInverse , vec2 InScreenPos , vec2 InScreenWH, float InFov , vec2 InNearFar , float InPrjPlaneWInverseH,float InCameraDepth)
{
    //gl_FragCoord  是以屏幕的左下角为原点的 并且在摄像机坐标系中 摄像机朝向的方向为Z的负方向
   vec2 PrjPlaneHalfWH ;
   PrjPlaneHalfWH.x = InNearFar.x * tan(InFov/2.0) ;
   PrjPlaneHalfWH.y = PrjPlaneHalfWH.x / InPrjPlaneWInverseH ;
   vec2 Pos = InScreenPos.xy /InScreenWH.xy * (PrjPlaneHalfWH * 2.0) ;
   Pos.x = Pos.x -  PrjPlaneHalfWH.x ;
   Pos.y = Pos.y - PrjPlaneHalfWH.y ;
   vec3 CameraPrjPlanePos = vec3(Pos.xy , -InNearFar.x) ;
   vec4 CameraSpacePos = vec4(CameraPrjPlanePos * (-InCameraDepth/InNearFar.x) , 1.0);
   //return CameraSpacePos ;
   return InViewInverse * CameraSpacePos ;
}

float HdrWeightY(float InY , float InExposureScale) 
{
  return 1.0/(InY * InExposureScale + 1.0);
}



void Bicubic2DCatmullRom( in vec2 UV, in vec2 Size, out vec2 Sample[3], out vec2 Weight[3] )
{
	const vec2 InvSize = 1.0 / Size;

	UV *= Size;

	vec2 tc = floor( UV - 0.5 ) + 0.5;
	vec2 f = UV - tc;
	vec2 f2 = f * f;
	vec2 f3 = f2 * f;
    
	vec2 w0 = f2 - 0.5 * (f3 + f);
	vec2 w1 = 1.5 * f3 - 2.5 * f2 + 1;
	vec2 w3 = 0.5 * (f3 - f2);
	vec2 w2 = 1 - w0 - w1 - w3;

	Weight[0] = w0;
	Weight[1] = w1 + w2;
	Weight[2] = w3;

	Sample[0] = tc - 1;
	Sample[1] = tc + w2 / Weight[1];
	Sample[2] = tc + 2;

	Sample[0] *= InvSize;
	Sample[1] *= InvSize;
	Sample[2] *= InvSize;
}

vec4  Texture2DSampleBicubic( sampler2D Tex ,vec2 UV, vec2 Size )
{
	vec2 Weight[3];
	vec2 Sample[3];
	Bicubic2DCatmullRom( UV, Size, Sample, Weight );

	vec4  OutColor;
	OutColor =  texture2D(Tex , vec2( Sample[1].x, Sample[0].y)) * Weight[1].x * Weight[0].y;
	OutColor += texture2D(Tex , vec2( Sample[0].x, Sample[1].y )) * Weight[0].x * Weight[1].y;
	OutColor += texture2D(Tex , vec2( Sample[1].x, Sample[1].y )) * Weight[1].x * Weight[1].y;
	OutColor += texture2D(Tex , vec2( Sample[2].x, Sample[1].y )) * Weight[2].x * Weight[1].y;
	OutColor += texture2D(Tex , vec2( Sample[1].x, Sample[2].y )) * Weight[1].x * Weight[2].y;


	float CornerWeights;
	CornerWeights = Weight[1].x * Weight[0].y;
	CornerWeights += Weight[0].x * Weight[1].y;
	CornerWeights += Weight[1].x * Weight[1].y;
	CornerWeights += Weight[2].x * Weight[1].y;
	CornerWeights += Weight[1].x * Weight[2].y;
	OutColor /= CornerWeights;

	return OutColor;
}

vec2 DecodeVelocityFormTexture(vec2 InVelocity)
{
   return InVelocity ;
}
vec3 tonemap(vec3 Color)
{
  return Color/(Color + vec3(1.0 , 1.0 , 1.0));
}
vec3 inverseTonemap(vec3 Color)
{
  return Color /(vec3(1.0 , 1.0 , 1.0) - Color);
}
void main()
{
	vec2 ScreenPos = gl_FragCoord.xy / vec2(Width , Height) ;
	vec2 PixelStepSize = vec2(1.0f / Width , 1.0f / Height);
	vec3 MinDepthPos;
	MinDepthPos.xy = textureCoord.xy ;
	MinDepthPos.z = texture2D(BBuffer, textureCoord.xy).z ;
	
   
   vec4 Neighbor[9];
    Neighbor[0] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (-1.0 , 1.0));
    Neighbor[1] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (0.0 , 1.0));
    Neighbor[2] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (1.0 , 1.0));
    Neighbor[3] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (-1.0 , 0.0));
    Neighbor[4] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (0.0 , 0.0));
    Neighbor[5] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (1.0 , 0.0));
    Neighbor[6] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (-1.0 , -1.0));
    Neighbor[7] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (0.0 , -1.0));
    Neighbor[8] = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (1.0 , -1.0));
   vec4 MinNeighbor = Neighbor[0];
   vec4 MaxNeighbor = Neighbor[0];
   for(int i = 0 ; i < 9; i++)
   {
        MaxNeighbor.rgb = max(MaxNeighbor.rgb , Neighbor[i].rgb);
		MinNeighbor.rgb = min(MinNeighbor.rgb , Neighbor[i].rgb);
   }
   
   
   
   
   
	vec4 Depths ; 
	Depths.x = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(2.0 ,2.0)).z ;
	Depths.y = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(-2.0 ,2.0)).z;
	Depths.z = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(2.0 ,-2.0)).z;
	Depths.w = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(-2.0 ,-2.0)).z;
	
	vec2 Offset = vec2(2.0f, 2.0f) ;
	float TmpX = Depths.x ;
	if(Depths.x > Depths.y) 
	{
	  Offset = vec2(-2.0 , 2.0) ;
	  TmpX = Depths.y ;
	}
	if(Depths.z < TmpX)
	{
	  Offset = vec2(2.0 , -2.0) ;
	  TmpX = Depths.z ;
	}
	if(Depths.w < TmpX) 
	{
	  Offset = vec2(2.0 , -2.0) ;
	  TmpX = Depths.z ;
	}
	if(TmpX < MinDepthPos.z)
	{
	  MinDepthPos.z = TmpX ;
	}
	else
	{
	  Offset = vec2(0.0 , 0.0) ;
	}
	
   
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, MinDepthPos.z) ;	
   vec4 PreClipCoord = PreProjectMatrix * PreViewMatrix * WorldPos ;
   
   
   PreClipCoord = PreClipCoord / PreClipCoord.w ;
   vec2 NDCCoord = MinDepthPos.xy * 2.0 - vec2(1.0 ,1.0);
    // OpenGL的NDC坐标系为-1 到 1
   // 以上所有代码都只是改变了深度 并没有改变ScreenPos  目的是当整个场景静态不动的时候 采用该像素周围最深的像素来计算BackVec 来对History采样
   vec2 Velocity = texture2D(VelocityRT , MinDepthPos.xy + Offset * PixelStepSize).rg ;
   vec2 BackVec = DecodeVelocityFormTexture(Velocity); // Unreal中使用了一种编码方式 使得使用更少的位数来存储Veclocity
   BackVec = NDCCoord - BackVec ;
   BackVec = BackVec.xy * 0.5 + vec2(0.5);
   PreClipCoord.xy = BackVec;
   // 对History采样  这里使用的是双三线采样算法  可以仔细看一下
   vec4 HistoryColor = Texture2DSampleBicubic(HistoryRT , BackVec.xy,vec2(Width , Height));
   HistoryColor = texture2D(HistoryRT , BackVec.xy );
   HistoryColor.rgb = clamp(HistoryColor.rgb , MinNeighbor.rgb , MaxNeighbor.rgb);
   vec4 CurrentColor = Neighbor[4];
   
   
   
   	// the linear filtering can cause blurry image, try to account for that:
	
	float SubpixelCorrection =  max(abs(Velocity.x)*Width, abs(Velocity.y)*Height) ;
	SubpixelCorrection = (SubpixelCorrection - int(SubpixelCorrection)) * 0.5f;

	// compute a nice blend factor:
	float blendfactor = 0.05f + (0.8f - 0.05f) * SubpixelCorrection;
    if(blendfactor < 0.0) blendfactor = 0.0;
	if(blendfactor > 1.0) blendfactor = 1.0;
	
	// if information can not be found on the screen, revert to aliased image:
	bool IsInVaild = false;
	if((PreClipCoord.x - clamp(PreClipCoord.x , 0.0f , 1.0f)) != 0.0f)
	   IsInVaild = true;
	if((PreClipCoord.y - clamp(PreClipCoord.y , 0.0f , 1.0f)) != 0.0f)
	   IsInVaild = true;
	   
	blendfactor = IsInVaild ? 1.0f : blendfactor;
	
	HistoryColor.rgb = tonemap(HistoryColor.rgb);
	CurrentColor.rgb = tonemap(CurrentColor.rgb);

	// do the temporal super sampling by linearly accumulating previous samples with the current one:
	vec4 resolved = vec4(HistoryColor.rgb + (CurrentColor.rgb - HistoryColor.rgb) * blendfactor, 1);
	FinalAAResult.rgb = inverseTonemap(resolved.rgb);
	FinalAAResult.a = 1.0;
}