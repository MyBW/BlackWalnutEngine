
#version 430 core
in vec2 textureCoord ;
uniform sampler2D BBuffer;
uniform sampler2D FinalRenderResult;
layout(location = 0) out vec4 FinalFilterResult;
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
vec3 ComputerNormal(vec2 InNormalXY)
{
   vec3 Normal ;
   float sinThta = sin(InNormalXY.x) ;
   float cosThta = cos(InNormalXY.x) ;
   float sinPh = sin(InNormalXY.y ) ;
   float cosPh = cos(InNormalXY.y ) ;
   Normal.x = sinThta * cosPh ;
   Normal.y = sinThta * sinPh ;
   Normal.z = cosThta ;
   return Normal;
}

void main()
{	
	vec2 PixelStepSize = vec2(1.0f /ScreenWH.x , 1.0f / ScreenWH.y);
	vec4 BBufferData = texture2D(BBuffer, textureCoord.xy) ;
	float CameraSpaceDepth = BBufferData.z ;
	vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
    vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, CameraSpaceDepth) ;	
    vec3  Normal = ComputerNormal(vec2(BBufferData.xy)) ;
    Normal = normalize(Normal) ;
    vec3 ViewDirection = normalize(ViewPositionWorldSpace - WorldPos.xyz);
    vec3 R = reflect(-ViewDirection , Normal) ;
    R = normalize(R);
    float StepSize = 3.0;
    vec3 FinalColor = vec3(0.0);
	vec4 Intersection = WorldPos.xyz;
	Intersection.w = 1.0 ;
    while(true)
	{
	    Intersection.xyz = Intersection.xyz + R * StepSize ;
	    ProcessIntersection = ViewMatrix * Intersection ;
	    float IntersectionCameraDepth = ProcessIntersection.z ;
		ProcessIntersection = ProjectMatrix *  ProcessIntersection ;
		ProcessIntersection.xy = ProcessIntersection.xy / w ;
		ProcessIntersection.xy = ProcessIntersection.xy * 0.5 + vec2(0.5) ;
		if(ProcessIntersection.x < 0.0 || ProcessIntersection.x > 1.0 || ProcessIntersection.y > 1.0 || ProcessIntersection.x < 0.0)
		{
		  Intersection.xyz = Intersection.xyz - R * StepSize ;
		  StepSize /= 2.0 ;
		  if(StepSize < 0.001)
		  {
		    break;
		  }
		  continue ;
		}
		float CurrentDepth = texture2D(BBuffer,ProcessIntersection.xy).z ;
		if(CurrentDepth < IntersectionCameraDepth)
		continue ;
		if(CurrentDepth > IntersectionCameraDepth)
		{
		  Intersection.xyz = Intersection.xyz - R * StepSize ;
		  StepSize /= 2.0 ;
		  continue;
		}
	    FinalColor = texture2D(FinalRenderResult, ProcessIntersection.xy).xyz;
		break;
	}
    
    FinalFilterResult = texture2D(FinalRenderResult, textureCoord.xy).rgb + FinalColor ;
	FinalFilterResult.w = 1.0;
}