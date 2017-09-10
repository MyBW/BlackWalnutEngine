#version 430 core
in vec2 textureCoord ;
uniform sampler2D BaseColorMap ;
uniform sampler2D NormalMap ;
uniform sampler2D PositionMap;
uniform samplerCube IBL_Specular_Light;
uniform sampler2D  IBL_LUT;

uniform vec4 SHCoefficient[9];


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
	vec3  Result = 0;
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
		CachedSH.R.V0 = float4(SHCoefficient[ColorIndex + 0], SHCoefficient[ColorIndex + 4], SHCoefficient[ColorIndex + 8], SHCoefficient[ColorIndex + 12]);
		CachedSH.R.V1 = float4(SHCoefficient[ColorIndex + 16], SHCoefficient[ColorIndex + 20], SHCoefficient[ColorIndex + 24], SHCoefficient[ColorIndex + 28]);
		CachedSH.R.V2 = SHCoefficient[ColorIndex + 32];
		
		int ColorIndex = 1 ;
		CachedSH.G.V0 = float4(SHCoefficient[ColorIndex + 0], SHCoefficient[ColorIndex + 4], SHCoefficient[ColorIndex + 8], SHCoefficient[ColorIndex + 12]);
		CachedSH.G.V1 = float4(SHCoefficient[ColorIndex + 16], SHCoefficient[ColorIndex + 20], SHCoefficient[ColorIndex + 24], SHCoefficient[ColorIndex + 28]);
		CachedSH.G.V2 = SHCoefficient[ColorIndex + 32];
		
		int ColorIndex = 2 ;
		CachedSH.B.V0 = float4(SHCoefficient[ColorIndex + 0], SHCoefficient[ColorIndex + 4], SHCoefficient[ColorIndex + 8], SHCoefficient[ColorIndex + 12]);
		CachedSH.B.V1 = float4(SHCoefficient[ColorIndex + 16], SHCoefficient[ColorIndex + 20], SHCoefficient[ColorIndex + 24], SHCoefficient[ColorIndex + 28]);
		CachedSH.B.V2 = SHCoefficient[ColorIndex + 32];
		

		FThreeBandSHVector DiffuseTransferSH = CalcDiffuseTransferSH3(Normal, 1);
		
		vec3 OutDiffuseLighting = max( vec3(0,0,0), DotSH3RGB(CachedSH, DiffuseTransferSH)) / PI;
		
}


void main()
{

   vec4  BaseColorMapData  = texture2D(BaseColorMap, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Roughness = BaseColorMapData.a ;
   
   vec4  NormalMapData = texture2D(NormalMap, textureCoord.xy) ;
   vec3  Normal = ComputerNormal(vec2(NormalMapData.xy)) ;
   Normal = normalize(Normal) ;
   float Specular = NormalMapData.z ;

   vec4  PositionMapData = texture2D(PositionMap, textureCoord.xy);
   float CameraSpaceDepth = PositionMapData.r ;
   float Metalic = PositionMapData.g ;

   
   vec3 Albedo , RealSpecular ;
   ComputeAlbedoAndSpecular(BaseColor , Metalic , Specular , Albedo , RealSpecular) ;
   
   vec2 NF = vec2( -NearFar.x , -NearFar.y) ;
   vec3 ViewPos = ViewPositionWorldSpace ;
   vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, ScreenWH, FoV, NF, PrjPlaneWInverseH, CameraSpaceDepth) ;
   vec3 ViewDirection = normalize(ViewPos - WorldPos.xyz);
   
   
   
  // Use Diffuse Light Eve Map To Render
   //vec3 InF = fresnelSchlickRoughness(max(dot(Normal , ViewDirection), 0.0) , RealSpecular , Roughness) ;  // 使用不同的Freshnel
   //vec3 InKs = InF ;
   //vec3 InKd = vec3(1.0) - InKs ;
   //vec3 InDirectLightDiffuseColor = InKd * Albedo * texture(IBL_Diffuse_Light, Normal).rgb / PI;
   
  //Use SH To Simulate Diffuse Light 
     vec3 InDirectLightDiffuseColor = GetIndirectLightDifffuseColor(Normal);
    

   vec3 R = reflect(-ViewDirection , Normal) ;
   vec2 Brdf = texture(IBL_LUT, vec2( 1 - NoV , Roughness)).xy ; // LUT 有问题
   vec3 InDirectLightSpecularPart1 = textureLod(IBL_Specular_Light, R , Roughness * 6.0 /* Mip Num*/).rgb ;
   vec3 InDirectLightSpecularPart2 = InF * Brdf.x + Brdf.y ;
   vec3 InDirectLightSpecular = InDirectLightSpecularPart1 * InDirectLightSpecularPart2 ;
   vec3 FinalColor = InDirectLightDiffuseColor + InDirectLightSpecular ;
   
   float gamma = 2.2 ;
   FinalColor = FinalColor;
   FinalColor = FinalColor / (FinalColor + vec3(1.0)) ;
  // FinalColor = pow(FinalColor , vec3(1.0/gamma)) ;
  // gl_FragColor.xyz = FinalColor;
   gl_FragColor.a = 1.0;
}