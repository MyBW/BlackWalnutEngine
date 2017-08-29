#version 430 core
in vec2 textureCoord ;
uniform sampler2D BaseColorMap ;
uniform sampler2D NormalMap ;
uniform sampler2D PositionMap;
uniform sampler2D ShadowMap ;
uniform vec3 LightDirection ;
uniform vec4 LightColor ;
uniform vec3 ViewPositionWorldSpace ;

uniform int ShadowIndex ;
uniform vec4 FrustumShadowEndWorldSpace0 ;
uniform vec4 FrustumShadowEndWorldSpace1 ;
uniform vec4 FrustumShadowEndWorldSpace2 ;
uniform vec4 FrustumShadowEndWorldSpace3 ;



uniform mat4 LightViewMatrix ;
uniform mat4 LightProjectionMatrix ;

float CalcShadowFactor(vec4 LightSpacePos) ;
bool IsInTheShadowMap(vec4 WorldSpacePos) ;

void main()
{
   vec3 lightDir = normalize(LightDirection);
   lightDir = -lightDir ;
   vec4  baseColor  = texture2D(BaseColorMap, textureCoord.xy) ;
   vec4  normal = texture2D(NormalMap, textureCoord.xy) ;
   vec4  WorldSpacePos = vec4(texture2D(PositionMap, textureCoord.xy).xyz, 1) ;
   vec3 ViewDir = normalize(ViewPositionWorldSpace - WorldSpacePos.xyz) ;
   float CosTheta = dot(normal.xyz, lightDir) ;
   float CosThetaView = dot(normal.xyz , ViewDir) ;
   vec4 LightSpacePosition = LightProjectionMatrix * LightViewMatrix * WorldSpacePos ;
   if(CosTheta < 0)
   {
     CosTheta = 0.0 ;
   }
   //baseColor += LightColor ;
   baseColor.a = 1.0;
   //baseColor.rgb *= CosTheta ;
   //baseColor.rgb *= CosThetaView ;
   gl_FragColor = vec4(baseColor.xyz , 1.0) ;
}

float CalcShadowFactor(vec4 LightSpacePos)
{
   vec3  LightSpaceProjCoord = LightSpacePos.xyz / LightSpacePos.w ;
   vec2  ShadowTextUV ;
   ShadowTextUV.x = 0.5 * LightSpaceProjCoord.x + 0.5 ;
   ShadowTextUV.y = 0.5 * LightSpaceProjCoord.y + 0.5 ;
   float z = 0.5 * LightSpaceProjCoord.z + 0.5 ;
   float Depth = texture2D(ShadowMap , ShadowTextUV).x ;
   if(Depth < (z - 0.00005))
   {
      return 0.2 ;
   }
   else
   {
     return 1.0 ;
   }
}

bool IsInTheShadowMap(vec4 WorldSpacePos) 
{
    vec4 Star ;
	vec4 End ;
    if(ShadowIndex == 0)
	{
	  Star = FrustumShadowEndWorldSpace0 ;
	  End = FrustumShadowEndWorldSpace1 ;
	}
	if(ShadowIndex == 1)
	{
	  Star = FrustumShadowEndWorldSpace1 ;
	  End = FrustumShadowEndWorldSpace2 ;
	}
	if(ShadowIndex == 2)
	{
	  Star = FrustumShadowEndWorldSpace2 ;
	  End = FrustumShadowEndWorldSpace3 ;
	}
    
	
	
	vec3 Dir = End.xyz - Star.xyz ;
	float Len = length(Dir) ;
	Dir  = normalize(Dir) ;
	
	vec3 Test = WorldSpacePos.xyz - Star.xyz ;
	float TestLen = length(Test) ;
	Test = normalize(Test) ;
	
	if( dot(Test,Dir) > 0)
	{
	   if( TestLen * dot(Test,Dir) < Len )
	   {
	       return true ;
	   }
	}
	return false ;
}


