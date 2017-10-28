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
void ComputeAlbedoAndSpecular(in vec3 InBaseColor ,in float InMetalic ,in float InSpecular , out vec3 OutAlbedo, out vec3 OutSpecular)
{
    OutAlbedo = InBaseColor *( 1 - InMetalic) ;
    float DielectircSpecular = mix(0.0 , 0.08 , InSpecular) ;
    OutSpecular = mix(vec3(DielectircSpecular) , InBaseColor , InMetalic) ;
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

void main()
{

	// BaseColor:3 Specular :1
	// Normal:2 CameraSpaceDepth: 1
	// Roughness:1   Metalic: 1

	
   vec4  BaseColorMapData  = texture2D(ABuffer, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Specular = BaseColorMapData.w ;
   
   //Roughness = 0.1 ;
   
   vec4  NormalMapData = texture2D(BBuffer, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Normal = normalize(Normal) ;
   float CameraSpaceDepth = NormalMapData.z;
   

   vec4  EffectData = texture2D(CBuffer, textureCoord.xy);
   float Roughness = EffectData.r ;
   float Metalic = EffectData.g ;
   float Occlusion = EffectData.w ;
   
   vec3 Albedo , RealSpecular ;
   ComputeAlbedoAndSpecular(BaseColor , Metalic , Specular , Albedo , RealSpecular) ;
   
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec3 ViewPos = ViewPositionWorldSpace ;
   vec2 ClipXY = (vec2(1.0) - textureCoord.xy) * 2.0 - vec2(1.0);
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix, ClipXY, FoV, PrjPlaneWInverseH, CameraSpaceDepth);
   
   
   vec3 ViewDirection = normalize(ViewPos - WorldPos.xyz);
   vec3 H = normalize(ViewDirection + LightDirection) ;

   float NoL = max(dot(Normal , LightDirection) , 0.0) ;
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;
   float HoV = max(dot(H , ViewDirection), 0.0);
   vec3 F = FresnelSchlick( NoV ,RealSpecular) ;
   float GItem = GeometrySmith(Normal , ViewDirection , LightDirection , Roughness) ;
   float DItem = DistributionGGX(Normal , H, Roughness ) ;
   vec3 FinalSpecular =  F * GItem * DItem /(4 * NoL * NoV + 0.001); 

   vec3 Ks = F;
   vec3 Kd = vec3(1.0) - Ks ;
   vec3 Diffuse = Kd * Albedo / PI * vec3(1.0) * ( 1.0 - Occlusion);
   vec3 DirectLightColor = ( Diffuse + FinalSpecular) *LightColor * NoL;
   gl_FragColor.xyz = DirectLightColor;
   gl_FragColor.a = 1.0;
}