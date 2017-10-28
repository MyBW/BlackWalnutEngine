#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
uniform sampler2D BloomTexture;
uniform float AvgLum;
uniform float Key;
const vec4 Lumfact = vec4(0.27,0.67,0.06,0.0);
layout(location = 0) out vec4 DestTexture;


float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x)
{
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}


void main()
{
	vec4 Color = texture2D(SrcTexture, textureCoord.xy) ;
	float Lum = dot(Color , Lumfact) ;
	Color += texture2D(BloomTexture, textureCoord.xy);
	Color *= Key * Lum/AvgLum;
	
	float ExposureBias = 2.0f;
    Color.xyz = Uncharted2Tonemap(ExposureBias * Color.xyz);
	
    vec3 WhiteScale = 1.0f/Uncharted2Tonemap(vec3(W));
	Color.xyz = Color.xyz * WhiteScale ;
	//Color = Color/(Color + vec4(1.0)) ;
	float gamma = 2.2 ;
    Color.xyz = pow(Color.xyz , vec3(1.0/gamma)) ;
	DestTexture = Color ;
}
