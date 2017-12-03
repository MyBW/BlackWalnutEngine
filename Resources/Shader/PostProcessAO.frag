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
layout(binding = 1,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
};


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


vec4 FromScreenToCamera(vec2 ClipSpaceXY ,float InFov , float InPrjPlaneWInverseH,float InCameraDepth)
{
   vec2 Pos;
   Pos.xy = ClipSpaceXY;
   vec4 CameraSpacePos;
   CameraSpacePos.x = Pos.x * InPrjPlaneWInverseH * tan(InFov/2.0f) * InCameraDepth;
   CameraSpacePos.y = Pos.y * tan(InFov / 2.0f) * InCameraDepth;
   CameraSpacePos.z = InCameraDepth;
   CameraSpacePos.w = 1.0f;
   return CameraSpacePos;
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
   vec2 ClipXY = (vec2(1.0) - textureCoord.xy) * 2.0 - vec2(1.0);
   vec4 CameraPos = FromScreenToCamera(ClipXY, FoV, PrjPlaneWInverseH, CameraSpaceDepth);
   vec2 NoiseSacle = vec2(ScreenWH.x / 4, ScreenWH.y / 4) ;
   
   vec3 RandVec = texture2D(Noise , textureCoord * NoiseSacle).xyz ;
   
   vec3 Tangent = normalize(RandVec - Normal * dot(Normal , RandVec)) ;
   vec3 Bitangent = cross(Normal, Tangent) ;
   mat3 TBN = mat3(Tangent, Bitangent, Normal);
   float Occlusion = 0.0 ;
   float Radius = 1.0 ;
   vec4 Offset;
   for(int i = 0 ; i < 64; i++)
   {
		vec3 Sample = TBN * SampleDirection[i];
		Sample = CameraPos.xyz + Sample * Radius;
		Offset = vec4(Sample , 1.0) ;
		Offset = ProjectMatrix * Offset ;
		Offset.xy /= Offset.w ;
		Offset.xy = Offset.xy * 0.5 + vec2(0.5);
		float SampleDepth = texture2D(BBufer, Offset.xy).b ;
		float RangeCheck = smoothstep(0.1 , 1.0 , Radius /abs(SampleDepth - CameraSpaceDepth));
		Occlusion += (SampleDepth <= Sample.z ? 0.0 : 1.0) * RangeCheck;
   }
   Occlusion /= 64 ; 
  AmbientOcclusion.xyz = vec3(0.0);
  AmbientOcclusion.w = Occlusion;
}