#version 430 core
in vec2 textureCoord ;
uniform sampler2D BBufer ;
uniform sampler2D Noise ;
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
   return CameraSpacePos ;
   //return InViewInverse * CameraSpacePos ;
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
    
   vec4  NormalMapData = texture2D(BBufer, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Normal = (ViewMatrix * vec4(Normal, 0.0)).xyz ;
   Normal = normalize(Normal) ;	
   float CameraSpaceDepth = NormalMapData.b;
   
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec3 ViewPos = ViewPositionWorldSpace ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, CameraSpaceDepth) ;
   //WorldPos.w = 1.0 ;
   vec4 CameraPos = ViewMatrix * WorldPos  ;
   CameraPos = WorldPos ;
   vec2 NoiseSacle = vec2(ScreenWH.x / 4, ScreenWH.y / 4) ;
   
   vec3 RandVec = texture2D(Noise , textureCoord * NoiseSacle).xyz ;
   
   vec3 Tangent = normalize(RandVec - Normal * dot(Normal , RandVec)) ;
   vec3 Bitangent = cross(Normal, Tangent) ;
   mat3 TBN = mat3(Tangent, Bitangent, Normal);
   float Occlusion = 0.0 ;
   float Radius = 1.0 ;
   vec4 Offset;
   
   vec2 NearHalfWH;
    NearHalfWH.x   = NF.x * tan(FoV/2.0) ;
   NearHalfWH.y = NearHalfWH.x / PrjPlaneWInverseH ;
   
   //CameraPos *= NF.x/CameraPos.z ;
   //CameraPos.xy /= NearHalfWH ;
   //CameraPos.xy = CameraPos.xy * 0.5 + 0.5;
   //CameraPos.x = 1- CameraPos.x ;
   //CameraPos.y = 1 - CameraPos.y ;
   
   for(int i = 0 ; i < 64; i++)
   {
		vec3 Sample = TBN * SampleDirection[i];
		Sample = CameraPos.xyz + Sample * Radius;
		Offset = vec4(Sample , 1.0) ;
		Offset *= NF.x/Offset.z ;
        Offset.xy /= NearHalfWH ;
		Offset.xy = Offset.xy * 0.5 + 0.5 ;
		Offset.xy = vec2(1.0)- Offset.xy ;
		float SampleDepth = texture2D(BBufer, Offset.xy).b ;
		float RangeCheck = smoothstep(0.1 , 1.0 , Radius /abs(SampleDepth - CameraSpaceDepth));
		Occlusion += (SampleDepth <= Sample.z ? 1.0 : 0.0) * RangeCheck;
   }
   Occlusion /= 64 ;
   //CameraPos.z = Occlusion ;
   //CameraPos.w = 1.0 ;
   //ProjPos = ProjectMatrix * CameraPos ;
   //ProjPos.xyz = ProjPos.xyz / ProjPos.w ;
   //ProjPos.xyz = ProjPos.xyz * 0.5 + 0.5 ;
   
   
   
  AmbientOcclusion.xyz = vec3(0.0);
  //if( abs(ProjPos.x - textureCoord.x ) < 0.2)
  if( Offset.x > 0.9)
  AmbientOcclusion.w = 1.0 ;
  else
  AmbientOcclusion.w = 0 ;
  AmbientOcclusion.w = Occlusion;
}