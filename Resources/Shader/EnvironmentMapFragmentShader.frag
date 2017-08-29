#version 430 core
uniform vec4 tmpColor ;
in vec2 textureCoord ;
in vec3 normal;
in vec4 woldPosition;
in vec3 EnvTextureCoord ;
uniform sampler2D tex1 ;
uniform sampler2D tex2;
uniform samplerCube CubeMapTexture;
uniform vec3 FreshnelF0 ;
uniform float RoughnessBase ;
uniform vec3 View ;

layout(location = 0) out vec4 BaseColorMap;
layout(location = 1) out vec4 NormalMap;
layout(location = 2) out vec4 PositionMap;
float RadicalInverse(int Base , int i) ;
float Hammersley(int Dimension , int Index , int NumberSamples);
vec3 SpecularIBL(vec3 SpecularColor , float Roughness , vec3 N , vec3 V) ;
const float PI = 3.1415926 ;
void main()
{
   vec4  TexColor  = texture2D(tex1 , textureCoord.xy) ;
   TexColor = texture2D(tex2, textureCoord.xy) ;
   NormalMap.rgb = normalize(normal); ;
   PositionMap.rgb = woldPosition.xyz ;
   vec4 BaseColor = vec4(1.0 ,1.0 ,1.0 , 1.0) ;
   vec3 Specular = FreshnelF0 ;
   Specular.r = 1.0;
   Specular.g = 0.72 ;
   Specular.b = 0.28 ;
   float Roughness = RoughnessBase;
    //Roughness = 0.0;
   vec3 SpecularIBLValue = SpecularIBL(Specular , Roughness , normalize(normal) , normalize(View)).rgb;
   BaseColorMap =  vec4(SpecularIBLValue.rgb, 1.0) ;
   BaseColorMap = vec4(TexColor.rgb, 1.0);

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

float D(float Roughness , float NDotH)
{
	float alpha2 =  Roughness * Roughness ;
	float beta = ( NDotH * NDotH * (alpha2 - 1) + 1) ;
    return alpha2 / (PI * beta * beta) ;
}

float G(float Roughness, float nDotV, float nDotL)
{
    float k = (Roughness + 1 ) * (Roughness + 1 )  / 8.0;
    float OneMunK = 1 -  k ;
    return (nDotV/(nDotV*OneMunK + k)) *(nDotL/(nDotL*OneMunK + k)) ;
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
	     vec3 EveColor = texture(CubeMapTexture, L).rgb; 
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
