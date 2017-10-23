#version 430 core
in vec2 textureCoord ;
uniform sampler2D ABuffer;
uniform sampler2D BBuffer;
uniform sampler2D CBuffer;
uniform sampler2D RayTrackBuffer;
uniform sampler2D FinalRenderResult; 

uniform float MipLevelNum ;
uniform float FadeEnd ;
uniform float FadeStart;
const float Max_Specular_Exp = 2.0;
layout(location = 0) out vec4 FinalFilterResult;
layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreModelMatix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
};
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};

vec4 FromScreenToWorld(mat4 InViewInverse , vec2 InScreenPos , vec2 InScreenWH, float InFov , vec2 InNearFar , float InPrjPlaneWInverseH,float InCameraDepth)
{
   vec2 Pos;
   Pos.xy = (vec2(1.0) - textureCoord.xy )* vec2(2.0) - vec2(1.0);
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
    vec3 sampleColor = texture2D(FinalRenderResult, samplePos, mipChannel).rgb;
    return vec4(sampleColor * gloss, gloss);
}

float IsoscelesTriangleNextAdjacent(float adjacentLength, float incircleRadius)
{
    // subtract the diameter of the incircle to get the adjacent side of the next level on the cone
    return adjacentLength - (incircleRadius * 2.0f);
}
float RoghnessToSpecularPower(float Roughness)
{
	return 1 - Roughness;
}
float saturate(float Value)
{
  return max(Value , 0.0f);
}


void ComputeAlbedoAndSpecular(in vec3 InBaseColor ,in float InMetalic ,in float InSpecular , out vec3 OutAlbedo, out vec3 OutSpecular)
{
    OutAlbedo = InBaseColor *( 1 - InMetalic) ;
    float DielectircSpecular = mix(0.0 , 0.08 , InSpecular) ;
    OutSpecular = mix(vec3(DielectircSpecular) , InBaseColor , InMetalic) ;
}

vec3 FresnelSchlick(float NdotV , vec3 F0)
{
  return F0 + (1.0 - F0)*pow(1.0 - NdotV , 5.0) ;
}



void main()
{
	vec4 BBufferData = texture2D(BBuffer, textureCoord.xy) ;
	float CameraSpaceDepth = BBufferData.z ;
	vec2 NF = vec2( NearFar.x , NearFar.y) ;
	NF.x =1;
	NF.y = 600;
	float NewFoV = 3.1415926/2.0;
	vec2 NewScreenWH = vec2(1024 , 768);
	float NewPrjPlaneWInverseH = 1.33333333;
	
    vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, NewScreenWH, NewFoV, NF, NewPrjPlaneWInverseH, CameraSpaceDepth) ;	
    vec3  Normal = ComputerNormal(vec2(BBufferData.xy)) ;
    Normal = normalize(Normal) ;
    vec4 CameraSpacePos = ViewMatrix * WorldPos;
	vec2 ScreenSpacePos = textureCoord.xy * ScreenWH;
	vec3 ViewPos = ViewPositionWorldSpace ;
    vec3 ViewDirection = normalize(ViewPos - WorldPos.xyz);
	
	
	vec4 IntersectionInfo = texture2D(RayTrackBuffer, textureCoord.xy);
	//IntersectionInfo.xy = vec2(0.0);
	if(IntersectionInfo.xy == vec2(0.0f))
	{
	  FinalFilterResult = textureLod(FinalRenderResult, textureCoord.xy ,0);
	  //FinalFilterResult.rgb = vec3(0.0);
	  return;
	}
	vec4 IntersectionBBuferInfo = texture2D(BBuffer, IntersectionInfo.xy);
	vec2 IntersectionScreenPos = IntersectionInfo.xy * ScreenWH;
	vec4 IntersectionWordPos = FromScreenToWorld(ViewInversMatrix , IntersectionScreenPos , NewScreenWH, NewFoV, NF, NewPrjPlaneWInverseH, IntersectionBBuferInfo.z) ;
	vec4 IntersectionCameraSpace = ViewMatrix * IntersectionWordPos ;
	
	vec2 Delta = IntersectionScreenPos - ScreenSpacePos;
	float AdjacentLenght = length(Delta);
	vec2 UnitAdjacent = normalize(Delta);
	
	vec4 EffectData = texture2D(CBuffer, textureCoord.xy);
	float Roughness = EffectData.r;
	float Gloss = 1.0 - Roughness ;
	float SpecularPower = RoghnessToSpecularPower(Roughness);
	float ConeTheta = SpecularPowerToConeTheta(SpecularPower);
	float MaxMipLevel = MipLevelNum - 1.0f; 
	
	
    vec4 FinalColor = vec4(0.0);
	float GlossMult = Gloss;
	float RemaingAlpha = 1.0f;
	for(float i = 0 ;i < 14; i++)
	{
		float OppositeLength = IsoscelesTriangleOpposite(AdjacentLenght , ConeTheta);
		float InRadius = IsoscelesTriangleInRadius(OppositeLength, AdjacentLenght);
		
		vec2 SamplePos = ScreenSpacePos + UnitAdjacent *(AdjacentLenght - InRadius);
		
		float MipLevel = clamp(log2(InRadius * max(ScreenWH.x , ScreenWH.y)), 0.0, MaxMipLevel);
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
	
	//FinalColor = ConeSampleWeightedColor(IntersectionInfo.xy, 0, 1);
	//RemaingAlpha -= GlossMult;
	
   vec4  BaseColorMapData  = texture2D(ABuffer, textureCoord.xy) ;
   vec3  BaseColor = BaseColorMapData.xyz ;
   float Specular = BaseColorMapData.w ;
   float Metalic = EffectData.g ;
   vec3 Albedo , RealSpecular ;
   ComputeAlbedoAndSpecular(BaseColor , Metalic , Specular , Albedo , RealSpecular) ;
   float NoV = max(dot(Normal , ViewDirection) , 0.0) ;
   vec3 F = FresnelSchlick( NoV ,RealSpecular) ;
   
   
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
    float fadeOnRoughness = min(saturate(Gloss * 4.0f) ,1.0);
    float totalFade = fadeOnBorder * fadeOnDistance * fadeOnPerpendicular * fadeOnRoughness * (1.0f - saturate(RemaingAlpha));
	
  // totalFade = fadeOnBorder * fadeOnDistance * fadeOnPerpendicular * fadeOnRoughness * (1.0f - saturate(RemaingAlpha));
  // FinalFilterResult.rgb = FinalColor.rgb  ;
  // FinalFilterResult.w = totalFade;
  // FinalFilterResult /= (FinalFilterResult + vec4(1.0)); 
  // return;
   
   
   vec3 fallbackColor = vec3(0.0);// IndirectLight
   fallbackColor = texture2D(FinalRenderResult, textureCoord.xy ,0).rgb;
   FinalColor.rgb = FinalColor.rgb * F ;//* totalFade;
   FinalColor.r = saturate(FinalColor.r);
   FinalColor.g = saturate(FinalColor.g);
   FinalColor.b = saturate(FinalColor.b);
   
   FinalFilterResult = vec4(fallbackColor + FinalColor.rgb , 1.0f);
   //FinalFilterResult.rgb = fallbackColor + FinalColor.rgb* totalFade;
   
   FinalFilterResult.w = 1.0f;
   //FinalFilterResult /= (FinalFilterResult + vec4(1.0)); 
   //FinalFilterResult = vec4(totalFade ,totalFade ,totalFade , length(FinalFilterResult.rgb));
   //FinalFilterResult.rgb = texture2D(FinalRenderResult, IntersectionInfo.xy,0).rgb;
   
}