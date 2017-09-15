#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
uniform float AvgLum;
uniform int Width;
uniform int Hight ;
layout(location = 0) out vec4 DestTexture;
const vec3 Effienc = vec3(0.27, 0.67,0.06);
void main()
{
	float dx = 1.0f/Width ;
	float dy = 1.0f/Hight ;
	vec4 Color = texture2D(SrcTexture , textureCoord.xy);
	Color += texture2D(SrcTexture , textureCoord.xy + vec2(dx*3.0f, 0.0f));
	
	Color += texture2D(SrcTexture,textureCoord.xy+vec2(0.0,dy));
	Color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*3.0,dy));
    
	Color += texture2D(SrcTexture,textureCoord.xy+vec2(0.0,dy*2.0));
	Color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*3.0,dy*2.0));
    
	Color += texture2D(SrcTexture,textureCoord.xy+vec2(0.0,dy*3.0));
	Color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*3.0,dy*3.0));
	
	Color /= 8.0;
	//计算该像素在Tone Mapping之后的亮度值，如果依然很大，则该像素将产生光晕
	vec4 Cout = vec4(0.0,0.0,0.0,0.0);
	float Lum = dot(Color.xyz , Effienc);
	vec4 p = Color * (Lum/AvgLum);
	p /= vec4(vec4(1.0,1.0,1.0,0.0)+p);
	float luml = (p.x+p.y+p.z)/3.0;
	if (luml < 0.8)
	{
	   Color = Cout ;
	}
	DestTexture = Color ;
}