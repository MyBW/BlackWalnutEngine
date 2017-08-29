#version 430 core
uniform vec4 tmpColor ;
in vec2 textureCoord ;
in vec3 normal;
in vec4 woldPosition;
in vec3 EnvTextureCoord ;
uniform sampler2D tex1 ;
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
   vec4 EveColor = texture(CubeMapTexture, EnvTextureCoord); 
   NormalMap.rgb = normalize(normal); ;
   PositionMap.rgb = woldPosition.xyz ;
   
   //BaseColorMap = TexColor * 0.5 + EveColor * 0.5 ;
   //BaseColorMap = EveColor ;
   //BaseColorMap = vec4(0.0 ,1.0 ,0.0 , 1.0) ;
  // EveColor.rgb = pow(EveColor.rgb , vec3(2.2)) ; //Gamma Correct
   //EveColor = vec4(0.5 , 0.5 ,0.5 , 1.0);
   //BaseColorMap = EveColor ;
   
   //vec3 SpecularIBLValue = SpecularIBL(FreshnelF0 , RoughnessBase , normalize(normal) ,View).rgb;
   //BaseColorMap = vec4(SpecularIBLValue.rgb, 1.0) ;
   //BaseColorMap = vec4(1.0 , 0.0, 0.0 , 1.0) ;
   BaseColorMap = TexColor ;
}




float GGX(float nDotV, float a)
{
float aa = a*a;
float oneMinusAa = 1 - aa;
float nDotV2 = 2 * nDotV;
float root = aa + oneMinusAa * nDotV * nDotV;
return nDotV2 / (nDotV + sqrt(root));
}

// shlick aproxx
//float GGX(float NdotV, float a)
//{
//float k = (a * a) / 2;
//return NdotV / (NdotV * (1.0f - k) + k);
//}

float G_Smith(float a, float nDotV, float nDotL)
{


    float Roughness2 = a * a ;
	float Vis_SmithV = nDotL *(nDotV *( 1- Roughness2) + Roughness2) ;
	float Vis_SmithL = nDotV *(nDotL *( 1- Roughness2) + Roughness2) ;
	return 0.5 / (Vis_SmithL + Vis_SmithV) ;
	//return GGX(nDotL,a) * GGX(nDotV,a);
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
   vec3 DiffuseLight = vec3(0.0 , 0.0 ,0.0) ;
   float Weight = 0 ;
   const int NumberSamples = 1024 ;
   for(int i = 0 ; i < NumberSamples ; i++)
   {
      vec2 Xi ;
	  Xi.x = Hammersley(0 , i, NumberSamples) ;
	  Xi.y = Hammersley(1 , i, NumberSamples) ;
	  vec3 H = ImportanceSampleGGX(Xi , Roughness , N) ;
	  vec3 L = 2 * dot(V , H) * H - V ;
	  float NoV = Saturate(dot(N , V)) ;
	  float NoL = Saturate(dot(N , L)) ;
	  float NoH = Saturate(dot(N , H)) ;
	  float VoH = Saturate(dot(V , H)) ;
	  
	  //float minV = min(NoL , min(NoV , NoH)) ;
	  //SpecularLighting += vec3(NoV) ;
	  if(NoL > 0)
	  {
	     vec3 EveColor = texture(CubeMapTexture, L).rgb; 
		 EveColor.rgb = pow(EveColor.rgb , vec3(2.2)) ; //Gamma Correct
		 float G = G_Smith(Roughness , NoV , NoL); //??????
		 float Fc = pow(1-VoH , 5) ;
		 vec3 F = (1 -Fc)* SpecularColor + Fc ;
		 //SpecularLighting += EveColor * F * G * min(1.0 ,max( VoH / NoH * NoV, 0.0)) ; 
		 SpecularLighting += EveColor * F * (NoL * G * (4 * VoH / NoH)) ; //???
		 //SpecularLighting += vec3(F) ;
		 //SpecularLighting += vec3(G) ;
		 //SpecularLighting += vec3(VoH) ;
		 
		 //DiffuseLight += EveColor * NoL ;
		 //Weight += NoL ;
		 
	  }
	  
   }
   
   return SpecularLighting / NumberSamples ; //+ (DiffuseLight / max(Weight , 0.001))  ;
}






