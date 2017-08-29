#version 430 core
in vec2 TextureCoord ;
in vec3 OutNormal;
in vec3 OutTangent ;
in float CameraSpaceDepth;
uniform sampler2D BaseColor ;
uniform sampler2D Roughness;
uniform sampler2D Metalic ;
uniform sampler2D Normal ;
uniform float Specular;

uniform vec3 TestBaseColor ;
uniform float TestRoughness ;
uniform float TestMetalic ;

out vec3  WorldPosition ;

layout(location = 0) out vec4 BaseColorMap;
layout(location = 1) out vec4 NormalMap;
layout(location = 2) out vec4 PositionMap;



layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
};



void main()
{
   // 正确的情况
   // BaseColor  And Roughness
   vec4  TexColor  = texture2D(BaseColor , TextureCoord.xy) ;
   BaseColorMap = TexColor ;
   BaseColorMap.a = texture2D(Roughness , TextureCoord.xy).r ;
   
   
   
   
   //Normal Map
  //Normal And Specular
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
   NormalMap.rg = vec2(acos(NormalFromNormalMap.z) , atan(NormalFromNormalMap.y , NormalFromNormalMap.x)) ;
   NormalMap.z = 0.5;

   //Normal And Specular
  /* vec3 VertexNormal = normalize(OutNormal) ;
   NormalMap.xy = vec2(acos(VertexNormal.z) , atan(VertexNormal.y , VertexNormal.x)) ;
   NormalMap.z = 0.5 ;
   */
   //Position At CameraSpaceDepth  And Metalic
   PositionMap.r = CameraSpaceDepth ;
   PositionMap.g = texture2D(Metalic , TextureCoord.xy).r;
}