#version 430 core
in vec2 textureCoord ;
uniform sampler2D ABuffer ;
uniform sampler2D BBuffer ;
uniform sampler2D CBuffer;
uniform sampler3D VoxelScene;
uniform vec3 PointLightPos ;
uniform vec3 PointLightColor;
uniform vec4 VoxelSize;
uniform vec3 SceneSizeMin;
uniform vec3 SceneSizeMax;
layout(location = 0) out vec4 ShadingReslut;
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
const float PI = 3.1415926 ;
#define TSQRT2 2.828427
#define SQRT2 1.414213
#define ISQRT2 0.707106
#define VOXEL_SIZE (1/64.0) /* Size of a voxel. 128x128x128 => 1/128 = 0.0078125. We ignor 0 mipmap level . so We Get 1/64.0*/
#define MIPMAP_HARDCAP 5.4f /* Too high mipmap levels => glitchiness, too low mipmap levels => sharpness. */
#define DIFFUSE_INDIRECT_FACTOR 0.5f /* Just changes intensity of diffuse indirect lighting. */

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(const vec3 p) { return 0.5f * p + vec3(0.5f); }


// Returns a vector that is orthogonal to u.
vec3 orthogonal(vec3 u){
  u = normalize(u);
  vec3 v = vec3(0.99146, 0.11664, 0.05832); // Pick any normalized vector.
  return abs(dot(u, v)) > 0.99999f ? cross(u, vec3(0, 1, 0)) : cross(u, v);
}

bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

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
   return  Specular + Diffuse;
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
   // This Funtion Is Just Replace, Render Reslut Is Very Bad! 
   float GItem = GeometrySmith(N , V , L , Roughness);
   vec3 Diffuse = DiffuseColor / PI ;
   return Diffuse * LobeEnergy[2] + (DItem * GItem) * F;
}




// Traces a diffuse voxel cone.
vec3 traceDiffuseVoxelCone(const vec3 from, vec3 direction){
  direction = normalize(direction);
  
  const float CONE_SPREAD = 0.325;

  vec4 acc = vec4(0.0f);

  // Controls bleeding from close surfaces.
  // Low values look rather bad if using shadow cone tracing.
  // Might be a better choice to use shadow maps and lower this value.
  float dist = 0.1953125;
  vec3 ImageSize = (SceneSizeMax - SceneSizeMin)/VoxelSize.xyz;
  // Trace.
  while(dist < SQRT2 && acc.a < 1){

    float TempDistanc  = dist * length(ImageSize);
    vec3 c = from + TempDistanc * direction;
    c= c.xyz / ((SceneSizeMax - SceneSizeMin)/2.0);
    c = scaleAndBias(c);

    float l = (1 + CONE_SPREAD * dist / VOXEL_SIZE);
    float level = log2(l);
    float ll = (level + 1) * (level + 1);
    vec4 voxel = textureLod(VoxelScene, c, min(MIPMAP_HARDCAP, level));
    acc += 0.075 * ll * voxel * pow(1 - voxel.a, 2);
    dist += ll * VOXEL_SIZE * 2;
  }
  return pow(acc.rgb * 2.0, vec3(1.5));
}


// Calculates indirect diffuse light using voxel cone tracing.
// The current implementation uses 9 cones. I think 5 cones should be enough, but it might generate
// more aliasing and bad blur.
vec3 indirectDiffuseLight( vec3 Normal, vec3 WorldPos){
  const float ANGLE_MIX = 0.5f; // Angle mix (1.0f => orthogonal direction, 0.0f => direction of normal).

  const float w[3] = {1.0, 1.0, 1.0}; // Cone weights.

  // Find a base for the side cones with the normal as one of its base vectors.
  const vec3 ortho = normalize(orthogonal(Normal));
  const vec3 ortho2 = normalize(cross(ortho, Normal));

  // Find base vectors for the corner cones too.
  const vec3 corner = 0.5f * (ortho + ortho2);
  const vec3 corner2 = 0.5f * (ortho - ortho2);

  // Find start position of trace (start with a bit of offset).
  const vec3 N_OFFSET = Normal * (1 + 4 * ISQRT2) * VOXEL_SIZE;
  const vec3 C_ORIGIN = WorldPos + N_OFFSET;

  // Accumulate indirect diffuse light.
  vec3 acc = vec3(0);

  // We offset forward in normal direction, and backward in cone direction.
  // Backward in cone direction improves GI, and forward direction removes
  // artifacts.
  const float CONE_OFFSET = -0.01;

  // Trace front cone
  acc += w[0] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * Normal, Normal);

  // Trace 4 side cones.
  const vec3 s1 = mix(Normal, ortho, ANGLE_MIX);
  const vec3 s2 = mix(Normal, -ortho, ANGLE_MIX);
  const vec3 s3 = mix(Normal, ortho2, ANGLE_MIX);
  const vec3 s4 = mix(Normal, -ortho2, ANGLE_MIX);

  acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho, s1);
  acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho, s2);
  acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * ortho2, s3);
  acc += w[1] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * ortho2, s4);

  // Trace 4 corner cones.
  const vec3 c1 = mix(Normal, corner, ANGLE_MIX);
  const vec3 c2 = mix(Normal, -corner, ANGLE_MIX);
  const vec3 c3 = mix(Normal, corner2, ANGLE_MIX);
  const vec3 c4 = mix(Normal, -corner2, ANGLE_MIX);

  acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner, c1);
  acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner, c2);
  acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN + CONE_OFFSET * corner2, c3);
  acc += w[2] * traceDiffuseVoxelCone(C_ORIGIN - CONE_OFFSET * corner2, c4);

  // Return result.
  return DIFFUSE_INDIRECT_FACTOR * acc ;
}


// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.
float traceShadowCone(vec3 from, vec3 direction, vec3 Normal, float targetDistance){

  from = from.xyz / ((SceneSizeMax - SceneSizeMin)/2.0);
  targetDistance = targetDistance /(SceneSizeMax).x;

  from += Normal * 0.05f; // Removes artifacts but makes self shadowing for dense meshes meh.

  float acc = 0;

  float dist = 3 * VOXEL_SIZE;
  // I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
  const float STOP = targetDistance - 16 * VOXEL_SIZE;

  while(dist < STOP && acc < 1){  
    vec3 c = from + dist * direction;
    if(!isInsideCube(c, 0)) break;
    c = scaleAndBias(c);
    float l = pow(dist, 2); // Experimenting with inverse square falloff for shadows.
    float s1 = 0.062 * textureLod(VoxelScene, c, 1 + 0.75 * l).a;
    float s2 = 0.135 * textureLod(VoxelScene, c, 4.5 * l).a;
    float s = s1 + s2;
    acc += (1 - acc) * s;
    dist += 0.9 * VOXEL_SIZE * (1 + 0.05 * l);
  }
  return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
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
   vec3 LightDirection = PointLightPos - WorldPos.xyz;
   float ToLightDistance = length(LightDirection);
   LightDirection = normalize(LightDirection) ;

   // 这几个参数都用来计算光照衰减的  目前我们只有AO衰减
   vec3 LobeRoughness;
   vec3 LobeEnergy ;
   float SurfaceAttenuation = 0.0f;
   LobeRoughness[1] = Roughness;
   LobeEnergy = vec3(1.0);
   
   
   //vec3 VoxelPos = WorldPos.xyz / ((SceneSizeMax - SceneSizeMin)/2.0);
   //VoxelPos = VoxelPos * 0.5 + vec3(0.5);
   //vec4 VoxelDiffuseColor = textureLod(VoxelScene, VoxelPos, 3);

  
   vec3 DirectLightShading ;
   float NoL = max(dot(Normal , LightDirection) , 0.0) ;
   //Unreal Shading 在Fresnel现象上表现更出色 在视线掠过物体表面时 能产生更亮的光斑 Bloom效果更好
   DirectLightShading = BWStanderShading(DiffuseColor, SpecularColor, LobeRoughness , LobeEnergy, LightDirection, ViewDirection , Normal);
   //DirectLightShading = StandarSahding(DiffuseColor, SpecularColor, LobeRoughness , LobeEnergy, LightDirection, ViewDirection , Normal);
   DirectLightShading = DirectLightShading * PointLightColor * NoL;


   vec3 InDirectDiffuseLighting;
   InDirectDiffuseLighting = indirectDiffuseLight(Normal, WorldPos.xyz) ;
   
   float ShadowFactor  ;
   ShadowFactor = traceShadowCone(WorldPos.xyz, LightDirection, Normal,  ToLightDistance);

   ShadingReslut.xyz = InDirectDiffuseLighting.xyz * 0.2 + DirectLightShading * 0.8 * ShadowFactor;
   ShadingReslut.a = 1.0;
}