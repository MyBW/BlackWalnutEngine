#version 430 core
in vec2 TextureCoord ;
in vec3 OutNormal;
in vec3 OutTangent ;
in float CameraSpaceDepth;
in vec4  ClipCoord ;
in vec4  PreClipCoord;

uniform sampler2D BaseColor ;
uniform sampler2D MacroVariation;
uniform sampler2D Normal ;

uniform vec3 TestBaseColor ;
uniform float TestRoughness ;
uniform float TestMetalic ;



layout(location = 0) out vec4 ABuffer;
layout(location = 1) out vec4 BBuffer;
layout(location = 2) out vec4 CBuffer;
layout(location = 3) out vec4 VelocityRT;

uniform mat4  ModelMatrix;
uniform mat4  PreModelMatrix;
layout(binding = 0,shared) uniform ViewportInformation
{
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
  mat4  ViewInversMatrix;
  vec3  ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2  NearFar;
  vec2  ScreenWH ;
};


void GetBaseColorAndRoughness(vec2 UVCoord, out vec3 FinalBaseColor, out float FinalRoughness)
{

   float MacroVariationValue1 = texture2D(MacroVariation , UVCoord * 0.2134).r + 0.5;
   float MacroVariationValue2 = texture2D(MacroVariation , UVCoord * 0.05341).r + 0.5;
   float MacroVariationValue3 = texture2D(MacroVariation , UVCoord * 0.002).r + 0.5;

   float MacroVariationValue = MacroVariationValue1 * MacroVariationValue2 * MacroVariationValue3 ;
   vec4 ColorValue = texture2D(BaseColor , UVCoord).bgra ;
   FinalBaseColor = (vec3(0.5, 0.5, 0.5) + vec3(0.5, 0.5, 0.5) * MacroVariationValue) * ColorValue.rgb;
   FinalRoughness = 0.6 + (0.4 - 0.6) * ColorValue.a * MacroVariationValue ;

}
// BaseColor:3 Specular :1
// Normal:2 CameraSpaceDepth: 1
// Roughness:1   Metalic: 1
// VelocityRT: 3
void main()
{
   // BaseColor  And Specular


   vec3 FinalBaseColor ;
   float FinalRoughness;
   GetBaseColorAndRoughness(TextureCoord.xy , FinalBaseColor, FinalRoughness);
   //vec4 ColorValue = texture2D(BaseColor , TextureCoord.xy) ;
   ABuffer.rgb = FinalBaseColor;
   ABuffer.a = 0.5;
   
   //Normal Map
  //Normal And CameraSpaceDepth
   vec3 NormalInTBN  ;
   vec3 tmpNormalInTBn  = texture2D(Normal , TextureCoord.xy).xyz;
   //法线贴图内存储的数据格式需要转换
   NormalInTBN.x = tmpNormalInTBn.z ;
   NormalInTBN.z = tmpNormalInTBn.x ;
   NormalInTBN.y = tmpNormalInTBn.y ;
   
   
   NormalInTBN = NormalInTBN * 2.0 - vec3(1.0);
   NormalInTBN = normalize(NormalInTBN) ;
   vec3 N = normalize(OutNormal) ;
   vec3 T = normalize(OutTangent);
   vec3 B = normalize(cross(N , T));
   vec3 NormalFromNormalMap ;
   NormalFromNormalMap.x = T.x * NormalInTBN.x + B.x * NormalInTBN.y + N.x * NormalInTBN.z ;
   NormalFromNormalMap.y = T.y * NormalInTBN.x + B.y * NormalInTBN.y + N.y * NormalInTBN.z ;
   NormalFromNormalMap.z = T.z * NormalInTBN.x + B.z * NormalInTBN.y + N.z * NormalInTBN.z ;
   
   NormalFromNormalMap = (ModelMatrix * vec4(NormalFromNormalMap , 0.0)).xyz;
   BBuffer.rg = vec2(acos(NormalFromNormalMap.z) , atan(NormalFromNormalMap.y , NormalFromNormalMap.x)) ;
   BBuffer.b = CameraSpaceDepth;
   

   //Roughness  And Metalic
   CBuffer.r = FinalRoughness ;
   CBuffer.g = 1.0;
   // Motion Blur
   vec3 NDCPos = (ClipCoord / ClipCoord.w).xyz ;
   vec3 PreNDXPos = (PreClipCoord / PreClipCoord.w).xyz;
   VelocityRT.xy = (NDCPos - PreNDXPos).xy;
}