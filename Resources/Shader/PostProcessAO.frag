#version 430 core
in vec2 textureCoord ;
niform sampler2D PositionMap ;
uniform sampler2D NoramlMap ;
uniform sampler2D NoiseMap ;
uniform vec3 SampleDirection[64];
layout(location = 0) out vec4 AmbientOcclusion ;
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};
layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
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


void main()
{
    
   vec4  NormalMapData = texture2D(NormalMap, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Noraml = (ViewMatrix * vec4(Normal, 0.0)).xyz ;
   Normal = normalize(Normal) ;	
   
   
   vec4  PositionMapData = texture2D(PositionMap, textureCoord.xy);
   float CameraSpaceDepth = PositionMapData.r ;
   
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec3 ViewPos = ViewPositionWorldSpace ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, CameraSpaceDepth) ;
   vec4 CameraPos = ViewMatrix * WorldPos  ;
   
   vec2 NoiseSacle = vec2(ScreenWH.x / 4, ScreenWH.y / 4) ;
   
   vec3 RandVec = texture2D(NoiseMap , textureCoord * NoiseSacle).xyz ;
   
   vec3 Tangent = normalize(RandVec - Noraml * dot(Normal , RandVec)) ;
   vec3 Bitangent = cross(Normal, Tangent) ;
   mat3 TBN = mat3(Tangent, Bitangent, Noraml);
   float Occlusion = 0.0 ;
   float Radius = 1.0 ;
   for(int i = 0 ; i < 64; i++)
   {
		vec3 Sample = TBN * SampleDirection[i];
		Sample = WorldPos + Sample * Radius ;
		vec4 Offset = vec4(Sample , 1.0) ;
		Sample = ProjectMatrix * Offset ;
		Sample = Sample.xyz / Sample.w ;
		Sample.xyz = Sample.xyz * 0.5 + 0.5 ;
		float SampleDepth = texture2D(PositionMap, Sample.xy).r ;
		float RangeCheck = smoothstep(0.0 , 1.0 , Radius /abs(SampleDepth - CameraSpaceDepth));
		Occlusion += (SampleDepth >= Sample.z ? 1.0 : 0.0)* RangeCheck;
   }
   Occlusion /= 64 ;
  AmbientOcclusion.xyz = 0.0;
  AmbientOcclusion.w = Occlusion ;
}