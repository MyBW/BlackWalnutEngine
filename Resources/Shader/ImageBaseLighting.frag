#version 430 core
in vec2 textureCoord ;
uniform sampler2D ABuffer ;
uniform sampler2D BBuffer ;
uniform sampler2D CBuffer;
uniform samplerCube IBL_Specular_Light;
uniform sampler2D  IBL_LUT;
layout(location = 0) out vec4 OutColor;
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};
uniform float SHCoefficient[36];
uniform float CubemapMaxMip;
const float PI = 3.1415926 ;


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



vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 




struct FThreeBandSHVector
{
	vec4  V0;
	vec4  V1;
	float  V2;
};


struct FThreeBandSHVectorRGB
{
	FThreeBandSHVector R;
	FThreeBandSHVector G;
	FThreeBandSHVector B;
};



float  DotSH3(FThreeBandSHVector A,FThreeBandSHVector B)
{
	float  Result = dot(A.V0, B.V0);
	Result += dot(A.V1, B.V1);
	Result += A.V2 * B.V2;
	return Result;
}

vec3  DotSH3RGB(FThreeBandSHVectorRGB A,FThreeBandSHVector B)
{
	vec3  Result = vec3(0.0, 0.0, 0.0);
	Result.r = DotSH3(A.R,B);
	Result.g = DotSH3(A.G,B);
	Result.b = DotSH3(A.B,B);
	return Result;
}



FThreeBandSHVector SHBasisFunction3( vec3  InputVector)
{
	FThreeBandSHVector Result;

	Result.V0.x = 0.282095f;
	Result.V0.y = -0.488603f * InputVector.y;
	Result.V0.z = 0.488603f * InputVector.z;
	Result.V0.w = -0.488603f * InputVector.x;

	vec3  VectorSquared = InputVector * InputVector;
	Result.V1.x = 1.092548f * InputVector.x * InputVector.y;
	Result.V1.y = -1.092548f * InputVector.y * InputVector.z;
	Result.V1.z = 0.315392f * (3.0f * VectorSquared.z - 1.0f);
	Result.V1.w = -1.092548f * InputVector.x * InputVector.z;
	Result.V2 = 0.546274f * (VectorSquared.x - VectorSquared.y);

	return Result;
}
FThreeBandSHVector CalcDiffuseTransferSH3( vec3 Normal, float  Exponent)
{
	FThreeBandSHVector Result = SHBasisFunction3(Normal);



	float  L0 = 2 * PI / (1 + 1 * Exponent );
	float  L1 = 2 * PI / (2 + 1 * Exponent );
	float  L2 = Exponent * 2 * PI / (3 + 4 * Exponent + Exponent * Exponent );
	float  L3 = (Exponent - 1) * 2 * PI / (8 + 6 * Exponent + Exponent * Exponent );


	Result.V0.x *= L0;
	Result.V0.yzw *= L1;
	Result.V1.xyzw *= L2;
	Result.V2 *= L2;

	return Result;
}

vec3 GetIndirectLightDifffuseColor(vec3 Normal)
{
	    FThreeBandSHVectorRGB CachedSH;
		int ColorIndex = 0 ;
		CachedSH.R.V0.xyzw = vec4(SHCoefficient[ColorIndex + 0], SHCoefficient[ColorIndex + 4], SHCoefficient[ColorIndex + 8], SHCoefficient[ColorIndex + 12]);
		CachedSH.R.V1 = vec4(SHCoefficient[ColorIndex + 16], SHCoefficient[ColorIndex + 20], SHCoefficient[ColorIndex + 24], SHCoefficient[ColorIndex + 28]);
		CachedSH.R.V2 = SHCoefficient[ColorIndex + 32];
		
		ColorIndex = 1 ;
		CachedSH.G.V0 = vec4(SHCoefficient[ColorIndex + 0], SHCoefficient[ColorIndex + 4], SHCoefficient[ColorIndex + 8], SHCoefficient[ColorIndex + 12]);
		CachedSH.G.V1 = vec4(SHCoefficient[ColorIndex + 16], SHCoefficient[ColorIndex + 20], SHCoefficient[ColorIndex + 24], SHCoefficient[ColorIndex + 28]);
		CachedSH.G.V2 = SHCoefficient[ColorIndex + 32];
		
		ColorIndex = 2 ;
		CachedSH.B.V0 = vec4(SHCoefficient[ColorIndex + 0], SHCoefficient[ColorIndex + 4], SHCoefficient[ColorIndex + 8], SHCoefficient[ColorIndex + 12]);
		CachedSH.B.V1 = vec4(SHCoefficient[ColorIndex + 16], SHCoefficient[ColorIndex + 20], SHCoefficient[ColorIndex + 24], SHCoefficient[ColorIndex + 28]);
		CachedSH.B.V2 = SHCoefficient[ColorIndex + 32];
		

		FThreeBandSHVector DiffuseTransferSH = CalcDiffuseTransferSH3(Normal, 1);
		
		vec3 OutDiffuseLighting = max( vec3(0,0,0), DotSH3RGB(CachedSH, DiffuseTransferSH));
		return OutDiffuseLighting;
		
}


float ComputeReflectionCaptureMipFromRoughness(float Roughness, float CubemapMaxMip)
{
   float LevelFrom1X1 = 1 - 1.2 * log2(Roughness);
   return CubemapMaxMip - 1 - LevelFrom1X1;
}


vec3 ComputeDiffuseColor(vec3 BaseColor , float Metalic)
{
  return BaseColor - BaseColor * Metalic;
}

vec3 ComputeSpecularColor(vec3 BaseColor , float Specular, float Metalic)
{
  return mix(vec3(Specular * 0.08) , BaseColor , Metalic);
}


void main()
{

 
   vec4  BaseColorMapData  = texture2D(ABuffer, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Specular = BaseColorMapData.w ;
   
   
   vec4  NormalMapData = texture2D(BBuffer, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Normal = normalize(Normal) ;
   float CameraSpaceDepth = NormalMapData.z;
   

   vec4  EffectData = texture2D(CBuffer, textureCoord.xy);
   float Metalic = EffectData.g ;
   float Roughness = EffectData.r ;
   float Occlusion = EffectData.w;


   vec3 DiffuseColor = ComputeDiffuseColor(BaseColor, Metalic);
   vec3 SpecularColor = ComputeSpecularColor(BaseColor ,Specular ,Metalic) ;
  
   

   vec3 ViewPos = ViewPositionWorldSpace ;   
   vec2 ClipXY = (vec2(1.0) - textureCoord.xy) * 2.0 - vec2(1.0);
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix, ClipXY, FoV, PrjPlaneWInverseH, CameraSpaceDepth);
   
   vec3 ViewDirection = normalize(ViewPos - WorldPos.xyz);
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;   
   
   vec3 InF = fresnelSchlickRoughness(max(dot(Normal , ViewDirection), 0.0) , SpecularColor , Roughness) ;  // This Freshnel Function Is Different With Direct Light 
   
  
   vec3 InKs = InF ;
   vec3 InKd = vec3(1.0) - InKs ;
   
  // Use Diffuse Light Eve Map To Render 
  //vec3 InDirectLightDiffuseColor = InKd * DiffuseColor * texture(IBL_Diffuse_Light, Normal).rgb / PI;
   
  //Use SH To Simulate Diffuse Light  This Is Just For SkyLighting 
  //   vec3 InDirectLightDiffuseColor = InKd * DiffuseColor* GetIndirectLightDifffuseColor(Normal)/ PI * Occlusion;
       vec3 InDirectLightDiffuseColor = vec3(0.0);

   vec3 R = reflect(-ViewDirection , Normal) ;

   vec2 Brdf = texture(IBL_LUT, vec2( 1 - NoV , Roughness)).xy ; // LUT 有问题


   float MipLevel = ComputeReflectionCaptureMipFromRoughness(Roughness , CubemapMaxMip);
   vec3 InDirectLightSpecularPart1 = textureLod(IBL_Specular_Light, R , MipLevel).rgb ;
   vec3 InDirectLightSpecularPart2 = InF * Brdf.x + Brdf.y ;                        // 其实这里直接用SpecularColor效果差别不大
   vec3 InDirectLightSpecular = InDirectLightSpecularPart1 * InDirectLightSpecularPart2 ;
   vec3 FinalColor = InDirectLightDiffuseColor + InDirectLightSpecular ;
   OutColor.xyz = FinalColor;
   OutColor.a = 0.0;
}