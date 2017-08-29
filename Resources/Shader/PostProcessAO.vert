#version 430 core
in vec2 textureCoord ;
uniform sampler2D NoramlMap ;
uniform sampler2D NoiseMap ;
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
void main()
{
   vec4 NormalMapData = texture2D(NormalMap , textureCoord.xy);
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   float CameraSpaceDepth = NormalMapData.z ;
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, CameraSpaceDepth) ;

   float RandVec = texture2D(NoiseMap , textureCoord * noisescale) ;

}