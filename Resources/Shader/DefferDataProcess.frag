#version 430 core
uniform float TestRoughness ;
in vec2 TextureCoord ;
in vec3 OutNormal;
in vec3 OutTangent ;
in float CameraSpaceDepth;
in vec4  ClipCoord ;
in vec4  PreClipCoord;

layout(location = 0) out vec4 ABuffer;
layout(location = 1) out vec4 BBuffer;
layout(location = 2) out vec4 CBuffer;
layout(location = 3) out vec4 VelocityRT;

layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreModelMatrix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
};

// BaseColor:3 Specular :1
// Normal:2 CameraSpaceDepth: 1
// Roughness:1   Metalic: 1
// VelocityRT: 3
void main()
{
   // 正确的情况
   // BaseColor  And Specular
   //vec4  TexColor  = vec4(1.0 ,0.79 ,0.21 , 0.0) ;
   vec4  TexColor  = vec4(1.0 ,1.0 ,1.0 , 0.0) ;
   ABuffer = TexColor ;
   ABuffer.a = 0.5;
   
   //Normal Map
  //Normal And CameraSpaceDepth
   vec3 NormalFromNormalMap;
   NormalFromNormalMap = (ModelMatrix * vec4(OutNormal , 0.0)).xyz;
   BBuffer.rg = vec2(acos(NormalFromNormalMap.z) , atan(NormalFromNormalMap.y , NormalFromNormalMap.x)) ;
   BBuffer.b = CameraSpaceDepth;
   

   //Roughness  And Metalic
   CBuffer.r = TestRoughness;
   CBuffer.g = 0.2;
   CBuffer.bw = vec2(0.0);
   // Motion Blur
   vec3 NDCPos = (ClipCoord / ClipCoord.w).xyz ;
   vec3 PreNDXPos = (PreClipCoord / PreClipCoord.w).xyz;
   VelocityRT.xy = (NDCPos - PreNDXPos).xy;
}