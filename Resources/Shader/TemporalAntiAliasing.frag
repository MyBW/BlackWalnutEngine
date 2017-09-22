#version 430 core
in vec2 textureCoord ;
uniform sampler2D BBuffer;
uniform sampler2D VelocityRT ;
uniform sampler2D HistoryRT;
uniform sampler2D FinalRenderResult;
uniform int Width ;
uniform int Height ;
uniform float PlusWeights[5] ;
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
  rcp(InY * InExposureScale + 1.0);
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



void main()
{
	vec2 ScreenPos = gl_FragCoord.xy / vec2(Width , Height) ;
	vec2 PixelStepSize = vec2(1.0f / Width , 1.0f / Height);
	vec3 MaxDepthPos.xy = ScreenPos ;
	MaxDepthPos.z = texture2D(BBuffer, textureCoord.xy).r ;
	
	vec4 Depths.x = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(2.0 ,2.0)).r ;
	Depths.y = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(-2.0 ,2.0)).r;
	Depths.z = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(2.0 ,-2.0)).r;
	Depths.w = texture2D(BBuffer, textureCoord.xy + PixelStepSize * vec2(-2.0 ,-2.0)).r;
	
	vec2 Offset = vec2(2.0f, 2.0f) ;
	float TmpX = Depths.x ;
	if(Depths.x < Depths.y) 
	{
	  Offset = vec2(-2.0 , 2.0) ;
	  TmpX = Depths.y ;
	}
	if(Depths.z > TmpX)
	{
	  Offset = vec2(2.0 , -2.0) ;
	  TmpX = Depths.z ;
	}
	if(Depths.w > TmpX) 
	{
	  Offset = vec2(2.0 , -2.0) ;
	  TmpX = Depths.z ;
	}
	if(TmpX > MaxDepthPos.z)
	{
	  MaxDepthPos.z = TmpX ;
	}
	else
	{
	  Offset = vec2(0.0 , 0.0) ;
	}
	
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , MaxDepthPos.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, MaxDepthPos.z) ;	
   vec4 PreClipCoord = PreProjectMatrix * PreViewMatrix * WorldPos ;
   PreClipCoord = PreClipCoord / PreClipCoord.w ;
   vec2 BackVec = ((MaxDepthPos.xy * 2.0).xy - vec2(1.0 ,1.0))).xy - PreClipCoord.xy  ;  // OpenGL的NDC坐标系为-1 到 1
   // 以上所有代码都只是改变了深度 并没有改变ScreenPos  目的是当整个场景静态不动的时候 采用该像素周围最深的像素来计算BackVec 来对History采样
   
   vec2 Velocity = texture2D(VelocityRT , MaxDepthPos.xy).rg ;
   bool IsDynamic = Velocity.x > 0.0f ;
   if(IsDynamic)//当场景是动态的时候 使用Velocity 来表示采样
   {
       BackVec = DecodeVelocityFormTexture(Velocity);
   }
   BackVec = ((MaxDepthPos.xy * 2.0).xy - vec2(1.0 ,1.0))).xy - BackVec ;
   // 这里需要对BackVec进行处理  看看是否超出边界
   bool OffScreen = max(abs(BackVec.x) , abs(BackVec.y)) > 1.0f;
   BackVec.x = clamp(BackVec.x , -1.0, 1.0) ;
   BackVec.y = clamp(BackVec.y, -1.0, 1.0) ;
   BackVec = BackVec.xy * 0.5 + 1.0 ;
   // 对当前的FinalResut采样
   vec4 Neighbor1 = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (0.0 , 1.0));
   vec4 Neighbor3 = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (-1.0 , 0.0));
   vec4 Neighbor4 = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (0.0 , 0.0));
   vec4 Neighbor5 = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (1.0 , 0.0));
   vec4 Neighbor7 = texture2D(FinalRenderResult, textureCoord.xy + PixelStepSize * (0.0 , -1.0));
   Neighbor1.rgb = RGBToYCoCg(Neighbor1.rgb) ;
   Neighbor3.rgb = RGBToYCoCg(Neighbor3.rgb) ;
   Neighbor4.rgb = RGBToYCoCg(Neighbor4.rgb) ;
   Neighbor5.rgb = RGBToYCoCg(Neighbor5.rgb) ;
   Neighbor7.rgb = RGBToYCoCg(Neighbor7.rgb) ;
   
   float Lum1 , Lum2 ;
   Lum1 = Neighbor1.x ; Lum2 = Neighbor1.x * Neighbor1.x ;
   Lum1 += Neighbor3.x ; Lum2 += Neighbor3.x * Neighbor3.x ;
   Lum1 += Neighbor4.x ; Lum2 += Neighbor4.x * Neighbor4.x ;
   Lum1 += Neighbor5.x ; Lum2 += Neighbor5.x * Neighbor5.x ;
   Lum1 += Neighbor7.x ; Lum2 += Neighbor7.x * Neighbor7.x ;
   float AvgLum1 = Lum1 * (1.0f/5.0) ;
   float VarLum = abs(Lum2 * (1.0/5) - pow(Lum1 ,2.0f)) ;// 这里猜测是使用的近似计算来得到方差的
   float NeighborWeight1 = HdrWeightY(Neighbor1.x , 1.0) ;
   float NeighborWeight3 = HdrWeightY(Neighbor3.x , 1.0) ;
   float NeighborWeight4 = HdrWeightY(Neighbor4.x , 1.0) ;
   float NeighborWeight5 = HdrWeightY(Neighbor5.x , 1.0) ;
   float NeighborWeight7 = HdrWeightY(Neighbor7.x , 1.0) ;
   Neighbor1.rgb *= NeighborWeight1;
   Neighbor3.rgb *= NeighborWeight3;
   Neighbor4.rgb *= NeighborWeight4;
   Neighbor5.rgb *= NeighborWeight5;
   Neighbor7.rgb *= NeighborWeight7;
   
   vec4 Filterd = Neighbor1 * PlusWeights[0] 
                + Neighbor3 * PlusWeights[1] 
				+ Neighbor4 * PlusWeights[2]
				+ Neighbor5 * PlusWeights[3]
				+ Neighbor7 * PlusWeights[4];
	if(VarLum > 1.0) Filterd = Neighbor4 ;
	
	vec2 TestPos = PixelStepSize + textureCoord.xy ;
	bool IsOffScreen = max(TestPos.x , TestPos.y)> 1.0 ;
	if(IsOffScreen) Filterd = Neighbor4 ;
	vec4 NeighborMin = min(min(min(min(Neighbor1.rgb , Neighbor3.rgb),Neighbor4.rgb), Neighbor5.rgb),Neighbor7.rgb);
	vec4 NeighborMax = max(max(max(max(Neighbor1.rgb , Neighbor3.rgb),Neighbor4.rgb), Neighbor5.rgb),Neighbor7.rgb);
	float NeighborAlphaMin = Neighbor1.a ;
	float NeighborAlphaMax = Neighbor1.a ;
	float NeighborMinWidght = NeighborWeight1 ;
	float NeighborMaxWidget = NeighborWidget1 ;
	WeightTrackedAlphaClamping(NeighborAlphaMin , NeighborMinWidget , NeighborAlphaMax , NeighborMaxWidget , Neighbor3.a , NeighborWeight3) ;
	WeightTrackedAlphaClamping(NeighborAlphaMin , NeighborMinWidget , NeighborAlphaMax , NeighborMaxWidget , Neighbor4.a , NeighborWeight4) ;
	WeightTrackedAlphaClamping(NeighborAlphaMin , NeighborMinWidget , NeighborAlphaMax , NeighborMaxWidget , Neighbor5.a , NeighborWeight5) ;
	WeightTrackedAlphaClamping(NeighborAlphaMin , NeighborMinWidget , NeighborAlphaMax , NeighborMaxWidget , Neighbor7.a , NeighborWeight7) ;
   
   // 对History采样  这里使用的是双三线采样算法  可以仔细看一下
   vec4 HistoryColor = Texture2DSampleBicubic(HistoryRT , BackVec.xy,vec2(Width , Hieght));
   HistoryColor.rgb = RGBToYCoCg(HistoryColor.rgb) ;
   float HistoryWidget = HdrWeightY(HistoryColor.x , 1.0) ;
   HistoryColor *= HistoryWidget ;
   
   bool IsDynamic1 = texture2D(VelocityRT, textureCoord.xy + PixelStepSize * (0.0 , 1.0));
   bool IsDynamic3 = texture2D(VelocityRT, textureCoord.xy + PixelStepSize * (-1.0 , 0.0));
   bool IsDynamic4 = texture2D(VelocityRT, textureCoord.xy + PixelStepSize * (0.0 , 0.0));
   bool IsDynamic5 = texture2D(VelocityRT, textureCoord.xy + PixelStepSize * (1.0 , 0.0));
   bool IsDynamic7 = texture2D(VelocityRT, textureCoord.xy + PixelStepSize * (0.0 , -1.0));
   bool IsAllDynamic = IsDynamic1 || IsDynamic3 || IsDynamic4 || IsDynamic5 || IsDynamic7;
   if(!IsAllDynamic && HistoryColor.a > 0)
   {
	  HistoryColor = Filterd ;
	  HistoryColor.a = 0 ;
   }
   // 混合 输出
   HistoryColor.rgb = clamp(HistoryColor.rgb , NeighborMin.rgb , NeighborMax.rgb);
   float BlendFinal = 1.0/4.0 ;  // Unreal中还提供了其他的算法 可去看看
   if(OffScreen)
   {
		HistoryColor = Filterd ;
		HistoryColor.a = 0 ;
   }
   vec4  OutColor.rgb = lerp(HistoryColor.rgb , Filterd.rgb , BlendFinal) ;
   OutColor.a = max(HistoryColor.a , 1.0/2.0);
   OutColor.rgb *= HdrWeightY(OutColor.x , 1.0);
   OutColor.rgb = YCoCgToRGB(OutColor.rgb);
   OutColor.rgb = -min(-OutColor.rgb, 0.0) ;
   OutColor.a = IsDynamic4 ? 1.0 : 0.0;
}