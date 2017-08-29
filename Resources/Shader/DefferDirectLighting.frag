#version 430 core
in vec2 textureCoord ;
uniform sampler2D BaseColorMap ;
uniform sampler2D NormalMap ;
uniform sampler2D PositionMap;
uniform samplerCube IBL_Diffuse_Light ;
uniform samplerCube IBL_Specular_Light;
uniform sampler2D  IBL_LUT;
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




float RadicalInverse(int Base , int i)
{
   float Digit , Radical , Inverse ;
   float DBase = Base ;
   Digit = Radical = 1.0/ DBase ;
   Inverse = 0.0 ;
   while(i != 0)
   {
      Inverse += Digit * (i % Base) ;
    Digit *= Radical ;
    i /= Base ;
   }
   return Inverse ;
}

float Hammersley(int Dimension , int Index , int NumberSamples)
{
    if(Dimension == 0)
  {
     float TmpNumberSamples = NumberSamples ;
     return  Index / TmpNumberSamples ;
  }
  else
  {
     return RadicalInverse(2 , Index) ;
  }
}

vec3 ImportanceSampleGGX(vec2 Xi , float Roughness , vec3 N)
{
    float a = Roughness * Roughness ;
  float PHi = 2 * PI  * Xi.x ;
  float CosTheta = sqrt((1-Xi.y) / (1 + (a * a - 1 ) * Xi.y)) ;
  float SinTheta = sqrt( 1 - CosTheta * CosTheta) ;
  vec3 H ;
  H.x = SinTheta * cos(PHi) ;
  H.y = SinTheta * sin(PHi) ;
  H.z = CosTheta ;
  
  float d = (CosTheta * a * a - CosTheta)* CosTheta + 1;
  float D = (a * a) /( PI * d * d) ;
  float PDF =  D * CosTheta ;
  
  vec3 UpVector  ;
  if(abs(N.z) < 0.999)
  {
    UpVector = vec3(0.0 ,0.0 ,1.0);
  }
  else
  {
    UpVector = vec3(1.0, 0.0, 0.0);
  }
   
  vec3 TangentX = normalize(cross(UpVector , N)) ;
  vec3 TangentY = cross(N , TangentX) ;
  
  return TangentX * H.x  + TangentY * H.y + N * H.z ;
}
float Saturate(float Value)
{  
   if(Value < 0.0) return 0.0 ;
   if(Value > 1.0) return 1.0 ;
   return Value ;
}

vec3 SpecularIBL(vec3 SpecularColor , float Roughness , vec3 N , vec3 V) 
{

   vec3 SpecularLighting = vec3(0.0 ,0.0 ,0.0) ;
   const int NumberSamples = 1024 ;
   for(int i = 0 ; i < NumberSamples ; i++)
   {
      vec2 Xi ;
    Xi.x = Hammersley(0 , i, NumberSamples) ;
    Xi.y = Hammersley(1 , i, NumberSamples) ;
    vec3 H = ImportanceSampleGGX(Xi , Roughness , N) ;
    H = normalize(H) ;
    vec3 L = 2 * dot(V , H) * H - V ;
    L =  normalize(L) ;
    float NoV = Saturate(dot(N , V)) ;
    float NoL = Saturate(dot(N , L)) ;
    float NoH = Saturate(dot(N , H)) ;
    float VoH = Saturate(dot(V , H)) ;
    float HoL = Saturate(dot(L , H)) ;
    if(NoL > 0)
    {
       vec3 EveColor ;//= texture(CubeMapTexture, L).rgb; 
       float Fc = pow(1- VoH, 5) ;
       vec3 FItem = SpecularColor + (1.0 - SpecularColor)*Fc ;
       float GItem = GeometrySmith(N , V , L , Roughness) ;
       float DItem = DistributionGGX(N , H, Roughness ) ;
     SpecularLighting += EveColor * FItem * GItem * DItem /(4 * NoL * NoV + 0.001) * NoL ; 
    }
    
   }
   SpecularLighting = SpecularLighting / NumberSamples ;
   return SpecularLighting ; 
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

void main()
{

   vec4  BaseColorMapData  = texture2D(BaseColorMap, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Roughness = BaseColorMapData.a ;
   
   //Roughness = 0.1 ;
   
   vec4  NormalMapData = texture2D(NormalMap, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Normal = normalize(Normal) ;
   float Specular = NormalMapData.z ;

   vec4  PositionMapData = texture2D(PositionMap, textureCoord.xy);
   float CameraSpaceDepth = PositionMapData.r ;
   float Metalic = PositionMapData.g ;

   //Metalic = 1.0;
   
   vec3 Albedo , RealSpecular ;
   ComputeAlbedoAndSpecular(BaseColor , Metalic , Specular , Albedo , RealSpecular) ;
   
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec3 ViewPos = ViewPositionWorldSpace ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, CameraSpaceDepth) ;
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
   vec3 Diffuse = Kd * Albedo / PI ;
   vec3 DirectLightColor = ( Diffuse + FinalSpecular) *LightColor * NoL;

   vec3 InF = fresnelSchlickRoughness(max(dot(Normal , ViewDirection), 0.0) , RealSpecular , Roughness) ;  // 使用不同的Freshnel
   vec3 InKs = InF ;
   vec3 InKd = vec3(1.0) - InKs ;

   vec3 InDirectLightDiffuseColor = InKd * Albedo * texture(IBL_Diffuse_Light, Normal).rgb / PI;

   vec3 R = reflect(-ViewDirection , Normal) ;
   vec2 Brdf = texture(IBL_LUT, vec2( 1 - NoV , Roughness)).xy ; // LUT 有问题
   vec3 InDirectLightSpecularPart1 = textureLod(IBL_Specular_Light, R , Roughness * 6.0 /* Mip Num*/).rgb ;
   vec3 InDirectLightSpecularPart2 = InF * Brdf.x + Brdf.y ;
   vec3 InDirectLightSpecular = InDirectLightSpecularPart1 * InDirectLightSpecularPart2 ;
   vec3 FinalColor = DirectLightColor  + InDirectLightDiffuseColor + InDirectLightSpecular ;
   
   float gamma = 2.2 ;
   FinalColor = FinalColor;
   FinalColor = FinalColor / (FinalColor + vec3(1.0)) ;
   FinalColor = pow(FinalColor , vec3(1.0/gamma)) ;
   gl_FragColor.xyz = FinalColor;
   gl_FragColor.a = 1.0;
}