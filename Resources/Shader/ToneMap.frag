#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
uniform sampler2D BloomTexture;
uniform float AvgLum ;
uniform float Key ;
const vec4 Lumfact = vec4(0.27,0.67,0.06,0.0);
layout(location = 0) out vec4 DestTexture;
void main()
{
	vec4 Color = texture2D(SrcTexture, textureCoord.xy) ;
	float Lum = dot(Color , Lumfact) ;
	Color += texture2D(BloomTexture, textureCoord.xy);
	
	Color *= Key *Lum/AveLum;
	Color = Color/(Color + vec4(1.0)) ;
	DestTexture = Color;
}
