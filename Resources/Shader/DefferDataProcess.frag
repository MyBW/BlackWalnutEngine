#version 430 core
in vec2 TextureCoord ;
in vec3 OutNormal;
in float CameraSpaceDepth;
uniform sampler2D BaseColor ;
uniform float Roughness ;
uniform float Specular ;
uniform float Metalic ;

out vec3  WorldPosition ;

layout(location = 0) out vec4 BaseColorMap;
layout(location = 1) out vec4 NormalMap;
layout(location = 2) out vec4 PositionMap;
void main()
{
   vec4  TexColor  = texture2D(BaseColor , TextureCoord.xy) ;
   BaseColorMap = TexColor ;
   BaseColorMap = vec4(1.0 ,0.79 ,0.21 , 0.0) ;
   BaseColorMap.a = Roughness ;
   
   vec3 Normal = normalize(OutNormal) ;
   NormalMap.rg = vec2(acos(Normal.z) , atan(Normal.y , Normal.x)) ;
   NormalMap.b = CameraSpaceDepth ;

   PositionMap.r = Specular ;
   PositionMap.g = Metalic ;
}