#version 430 core
in vec2 textureCoord ;
uniform sampler2D ABuffer;
uniform sampler2D BBuffer;
uniform sampler2D CBuffer;
uniform sampler2D RayTrackBuffer;
uniform sampler2D FinalRenderResult; 
uniform sampler2D IBL_LUT;

uniform float MipLevelNum ;
uniform float FadeEnd ;
uniform float FadeStart;
uniform float Max_Specular_Exp ;
layout(location = 0) out vec4 FinalFilterResult;
float PI = 3.1415926;

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
///Phong Light Mode .
float SpecularPowerToConeTheta(float InSpecularPower)
{
	if(InSpecularPower > exp2(Max_Specular_Exp))
	{
		return 0.0f;
	}
	const float xi = 0.2444f;
	float Exponent = 1.0/(InSpecularPower + 1.0f);
	return acos(pow(xi, Exponent));
}
////Phong Light Mode.
float RoghnessToSpecularPower(float Roughness , vec3 L, vec3 V, vec3 N)
{
  vec3 H = normalize(L + V) ;
  return pow(dot(N,H), 30.0 *( 1 - Roughness));
}

float IsoscelesTriangleOpposite(float adjacentLength, float coneTheta)
{
    // simple trig and algebra - soh, cah, toa - tan(theta) = opp/adj, opp = tan(theta) * adj, then multiply * 2.0f for isosceles triangle base
    return 2.0f * tan(coneTheta) * adjacentLength;
}

float IsoscelesTriangleInRadius(float a, float h)
{
    float a2 = a * a;
    float fh2 = 4.0f * h * h;
    return (a * (sqrt(a2 + fh2) - a)) / (4.0f * h);
}
vec4 ConeSampleWeightedColor(vec2 samplePos, float mipChannel, float gloss)
{
    vec3 sampleColor = textureLod(FinalRenderResult, samplePos, mipChannel).rgb;
    return vec4(sampleColor * gloss, gloss);
}

float IsoscelesTriangleNextAdjacent(float adjacentLength, float incircleRadius)
{
    // subtract the diameter of the incircle to get the adjacent side of the next level on the cone
    return adjacentLength - (incircleRadius * 2.0f);
}

float saturate(float Value)
{
  return max(Value , 0.0f);
}



vec3 ComputeDiffuseColor(vec3 BaseColor , float Metalic)
{
  return BaseColor - BaseColor * Metalic;
}

vec3 ComputeSpecularColor(vec3 BaseColor , float Specular, float Metalic)
{
  return mix(vec3(Specular * 0.08) , BaseColor , Metalic);
}

vec3 FresnelSchlick(float NdotV , vec3 F0)
{
  return F0 + (1.0 - F0)*pow(1.0 - NdotV , 5.0) ;
}



void main()
{
	vec4 BBufferData = texture2D(BBuffer, textureCoord.xy) ;
	float CameraSpaceDepth = BBufferData.z ;
  vec2 ClipXY = (vec2(1.0) - textureCoord.xy) * 2.0 - vec2(1.0);
  vec4 WorldPos = FromScreenToWorld(ViewInversMatrix, ClipXY, FoV, PrjPlaneWInverseH, CameraSpaceDepth); 	
  vec3  Normal = ComputerNormal(vec2(BBufferData.xy)) ;
  Normal = normalize(Normal) ;
  vec4 CameraSpacePos = ViewMatrix * WorldPos;
	vec2 ScreenSpacePos = textureCoord.xy * ScreenWH;
	vec3 ViewPos = ViewPositionWorldSpace ;
  vec3 ViewDirection = normalize(ViewPos - WorldPos.xyz);

	vec4 IntersectionInfo = texture2D(RayTrackBuffer, textureCoord.xy);
	if(IntersectionInfo.xyz == vec3(0.0f))
	{
	  FinalFilterResult = textureLod(FinalRenderResult, textureCoord.xy ,0);
	  return;
	}
   vec4 IntersectionBBuferInfo = texture2D(BBuffer, IntersectionInfo.xy);
   vec2 IntersectionScreenPos = IntersectionInfo.xy * ScreenWH;
   vec2 IntersectionClipXY = (vec2(1.0) - IntersectionScreenPos.xy) * 2.0 - vec2(1.0);
   vec4 IntersectionWordPos = FromScreenToWorld(ViewInversMatrix, IntersectionClipXY, FoV, PrjPlaneWInverseH, IntersectionBBuferInfo.z);
   vec4 IntersectionCameraSpace = ViewMatrix * IntersectionWordPos ;
   vec3 LightDirection = normalize(IntersectionWordPos.xyz - WorldPos.xyz);
	



  
	vec2 Delta = IntersectionScreenPos - ScreenSpacePos;
	float AdjacentLenght = length(Delta);
	vec2 UnitAdjacent = normalize(Delta);
	vec4 EffectData = texture2D(CBuffer, textureCoord.xy);
	float Roughness = EffectData.r;
	float Gloss = 1.0 - Roughness ;
	//float SpecularPower = RoghnessToSpecularPower(Roughness , LightDirection, ViewDirection, Normal);
	//float ConeTheta = SpecularPowerToConeTheta(SpecularPower) * 0.5;
	float ConeTheta =  PI/2.0 * pow(Roughness , 2.0)/2.0;
	float MaxMipLevel = MipLevelNum - 1.0f; 
	
	
    vec4 FinalColor = vec4(0.0);
	float GlossMult = Gloss;
	float RemaingAlpha = 1.0f;
	for(float i = 0 ;i < 14; i++)
	{
		float OppositeLength = IsoscelesTriangleOpposite(AdjacentLenght , ConeTheta);
		float InRadius = IsoscelesTriangleInRadius(OppositeLength, AdjacentLenght);
		
		vec2 SamplePos = ScreenSpacePos + UnitAdjacent *(AdjacentLenght - InRadius);
		
		float MipLevel = clamp(log2(InRadius), 0.0, MaxMipLevel);
		SamplePos /= ScreenWH;
		vec4 NewColor = ConeSampleWeightedColor(SamplePos, MipLevel, GlossMult);
		
		RemaingAlpha -= NewColor.a;
		if(RemaingAlpha < 0.0f)
		{
			NewColor.rgb *= (1.0f - abs(RemaingAlpha));
		}
		
		FinalColor += NewColor;
		if(FinalColor.a > 1.0)
		  break;
		
		AdjacentLenght = IsoscelesTriangleNextAdjacent(AdjacentLenght, InRadius);
		GlossMult *= Gloss;
	}
   vec4  BaseColorMapData  = texture2D(ABuffer, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Specular = BaseColorMapData.w ;
   float Metalic = EffectData.g ;
   vec3 SpecularColor ;
   SpecularColor = ComputeSpecularColor(BaseColor, Specular, Metalic);
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;
   vec2 Brdf = texture(IBL_LUT, vec2( 1 - NoV , Roughness)).xy ; // LUT 有问题
   vec3 F = SpecularColor * Brdf.x + Brdf.y;
  
   
   // fade rays close to screen edge
    //vec2 boundary = abs(IntersectionInfo.xy - vec2(0.5f, 0.5f)) * 2.0f;
    //const float fadeDiffRcp = 1.0f / (FadeEnd - FadeStart);
    //float fadeOnBorder = 1.0f - saturate((boundary.x - FadeStart) * fadeDiffRcp);
    //fadeOnBorder *= 1.0f - saturate((boundary.y - FadeStart) * fadeDiffRcp);
    //fadeOnBorder = smoothstep(0.0f, 1.0f, fadeOnBorder);
	
	
	vec2 boundary = abs(IntersectionInfo.xy - vec2(0.5f, 0.5f));
	float InvFadeBorder = 1.0/FadeStart;
    float fadeOnBorder = saturate(FadeStart - (0.5 - boundary.x)) * InvFadeBorder;
    fadeOnBorder *= saturate(FadeStart - (0.5 - boundary.y)) * InvFadeBorder;
    fadeOnBorder = smoothstep(0.0f, 1.0f, fadeOnBorder);
    
	
    float fadeOnDistance = 1.0f - saturate(distance(IntersectionCameraSpace, CameraSpacePos) / (NearFar.x - NearFar.y));
    // ray tracing steps stores rdotv in w component - always > 0 due to check at start of this method
    float fadeOnPerpendicular = min(1.0, saturate(saturate(IntersectionInfo.w * 4.0f)));
    float fadeOnRoughness = min(saturate(1- Roughness * 4.0f) ,1.0);
    float totalFade = fadeOnBorder * fadeOnDistance * fadeOnPerpendicular * fadeOnRoughness * (1.0f - saturate(RemaingAlpha));
	
	totalFade = fadeOnBorder * fadeOnDistance ;
 
    vec3 fallbackColor = vec3(0.0);// IndirectLight
   fallbackColor = textureLod(FinalRenderResult, textureCoord.xy ,0).rgb;
   FinalColor.rgb = FinalColor.rgb * F * fadeOnRoughness;  
   FinalFilterResult = vec4(fallbackColor + FinalColor.rgb , 1.0f); 
}