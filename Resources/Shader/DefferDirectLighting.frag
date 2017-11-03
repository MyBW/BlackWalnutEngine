#version 430 core
in vec2 textureCoord ;
uniform sampler2D ABuffer ;
uniform sampler2D BBuffer ;
uniform sampler2D CBuffer;
uniform vec3 LightDirection ;
uniform vec3 LightColor;
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};
const float PI = 3.1415926 ;
vec3 ComputeDiffuseColor(vec3 BaseColor , float Metalic)
{
  return BaseColor - BaseColor * Metalic;
}
vec3 ComputeSpecularColor(vec3 BaseColor , float Specular, float Metalic)
{
  return mix(vec3(Specular * 0.08) , BaseColor , Metalic);
}
float Saturate(float Value)
{  
   Value = max(Value , 0.0);
   Value = min(Value, 1.0);
   return Value;
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


vec3 FresnelSchlick(float NdotV , vec3 F0)
{
  return F0 + (1.0 - F0)*pow(1.0 - NdotV , 5.0) ;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
  
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
  
    return ggx1 * ggx2;
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
  
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
  
    return nom / denom;
}


vec4 FromScreenToWorld(mat4 InViewInverse , vec2 ClipSpaceXY ,float InFov , float InPrjPlaneWInverseH,float InCameraDepth)
{
   vec2 Pos;
   Pos.xy = ClipSpaceXY;
   vec4 CameraSpacePos;
   CameraSpacePos.x = Pos.x * InPrjPlaneWInverseH * tan(InFov/2.0f) * InCameraDepth;
   CameraSpacePos.y = Pos.y * tan(InFov / 2.0f) * InCameraDepth;
   CameraSpacePos.z = InCameraDepth;
   CameraSpacePos.w = 1.0f;
   return InViewInverse * CameraSpacePos;
}

vec3 BWStanderShading(vec3 DiffuseColor , vec3 SpecularColor, vec3 LobeRoughness , vec3 LoabeEnergy, vec3 L, vec3 V , vec3 N)
{
   vec3 H = normalize(V + L) ;
   float Roughness = LobeRoughness[1];
   float NoL = max(dot(N , L) , 0.0) ;
   float NoV = max(dot(N , V) , 0.0) ;
   float HoV = max(dot(H , V), 0.0);
   vec3 F = FresnelSchlick( NoV ,SpecularColor) ;
   float GItem = GeometrySmith(N , V , L , Roughness) ;
   float DItem = DistributionGGX(N , H, Roughness ) ;
   vec3 Specular =  F * GItem * DItem /(4 * NoL * NoV + 0.001); 

   vec3 Ks = F;
   vec3 Kd = vec3(1.0) - Ks ;
   vec3 Diffuse = Kd * DiffuseColor / PI ;

   return Specular + Diffuse;
}
//////Unreal Shading Code

vec3 F_Schlick(vec3 SpecularColor, float VoH)
{
  float Fc = pow(1.0 - VoH , 5.0);

  return vec3(Saturate(50.0 * SpecularColor.g) * Fc) +  (1.0 - Fc) * SpecularColor ;
}

float D_GGX(float Roughness, float NoH)
{
  float a = Roughness * Roughness;
  float a2 = a * a;
  float d = (NoH * a2 - NoH) * NoH + 1;
  return a2/(PI * d * d);
}
float Vis_Smith(float Roughness , float NoV, float NoL)
{
  float a = Roughness * Roughness;
  float a2 = a * a;
  float Vis_SmithV = NoV + sqrt(NoV * (NoV - NoV * a2) + a2) ;
  float Vis_SmithL = NoL + sqrt(NoL * (NoL - NoL * a2) + a2) ;
  return 1.0/(Vis_SmithV * Vis_SmithL);
}
float Vis_SmitJointApprox(float Roughness , float NoV, float NoL)
{
  float a = Roughness * Roughness;
  float Vis_SmithV = NoL *(NoV *(1 - a) + a) ;
  float Vis_SmithL = NoV *(NoL *(1 - a) + a);
  return 0.5 * 1.0/(Vis_SmithV + Vis_SmithL) ; 
}
vec3  StandarSahding(vec3 DiffuseColor , vec3 SpecularColor, vec3 LobeRoughness , vec3 LobeEnergy, vec3 L, vec3 V , vec3 N)
{
   float Roughness = LobeRoughness[1];
   vec3 H = normalize(V + L) ;
   float NoL = Saturate(dot(N , L)) ;
   float NoV = Saturate(dot(N , V) + 1e-5) ;
   float HoV = Saturate(dot(H , V));
   float HoN = Saturate(dot(H , N));
   vec3 F = F_Schlick( SpecularColor, HoV) ;
   float DItem = D_GGX(Roughness, HoN) * LobeEnergy[1];
   // Unreal Vis_Smith Will Return Bad Data On The Fianl Line .  Only When Light Direction Is vec3(0.0 , -1.0 ,-1.0) Is Return Right.
   // But This Function Can Give Better Reslut !
   //float GItem = Vis_Smith(Roughness, NoV, NoL) ;
   float GItem = GeometrySmith(N , V , L , Roughness);
   vec3 Diffuse = DiffuseColor / PI ;
   return Diffuse * LobeEnergy[2] + (DItem * GItem) * F;
}




void main()
{

	// BaseColor:3 Specular :1
	// Normal:2 CameraSpaceDepth: 1
	// Roughness:1   Metalic: 1  Occlusion: 1

	
   vec4  BaseColorMapData  = texture2D(ABuffer, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Specular = BaseColorMapData.w ;
   
   
   vec4  NormalMapData = texture2D(BBuffer, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Normal = normalize(Normal) ;
   float CameraSpaceDepth = NormalMapData.z;
   

   vec4  EffectData = texture2D(CBuffer, textureCoord.xy);
   float Roughness = EffectData.r ;
   float Metalic = EffectData.g ;
   float Occlusion = EffectData.w ;
   

   vec3 DiffuseColor = ComputeDiffuseColor(BaseColor, Metalic);
   vec3 SpecularColor = ComputeSpecularColor(BaseColor ,Specular ,Metalic) ;
   
   vec3 ViewPos = ViewPositionWorldSpace ;
   vec2 ClipXY = (vec2(1.0) - textureCoord.xy) * 2.0 - vec2(1.0);
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix, ClipXY, FoV, PrjPlaneWInverseH, CameraSpaceDepth);
   
   
   vec3 ViewDirection = normalize(ViewPos - WorldPos.xyz);
  

   // 这几个参数都用来计算光照衰减的  目前我们只有AO衰减
   vec3 LobeRoughness;
   vec3 LobeEnergy ;
   float SurfaceAttenuation = (1.0 - Occlusion) ;
   LobeRoughness[1] = Roughness;
   LobeEnergy = vec3(1.0);
   

   vec3 ShadingReslut ;
   float NoL = max(dot(Normal , LightDirection) , 0.0) ;
   //Unreal Shading 在Fresnel现象上表现更出色 在视线掠过物体表面时 能产生更亮的光斑 Bloom效果更好
   //ShadingReslut = BWStanderShading(DiffuseColor, SpecularColor, LobeRoughness , LobeEnergy, LightDirection, ViewDirection , Normal);
   ShadingReslut = StandarSahding(DiffuseColor, SpecularColor, LobeRoughness , LobeEnergy, LightDirection, ViewDirection , Normal);

   vec3 DirectLightColor = ShadingReslut * LightColor * NoL * SurfaceAttenuation;
   gl_FragColor.xyz = DirectLightColor;
   gl_FragColor.a = 1.0;
}