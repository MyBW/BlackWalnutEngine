//在Y轴上的高斯模糊
#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
uniform int Hight ;
layout(location = 0) out vec4 FilterResult;
void main()
{
	float d = 1.0/float(Hight);
	vec4 color = vec4(0.0,0.0,0.0,0.0);
	
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0,-5.0*d)) * 0.1;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0,-4.0*d)) * 0.22;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0,-3.0*d)) * 0.358;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0,-2.0*d)) * 0.563;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0,-1.0*d)) * 0.873;
	color +=     texture2D(SrcTexture ,textureCoord.xy		     ) * 1.0;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0, 1.0*d)) * 0.873;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0, 2.0*d)) * 0.563;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0, 3.0*d)) * 0.358;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0, 4.0*d)) * 0.22;
	color +=     texture2D(SrcTexture ,textureCoord.xy+vec2(0.0, 5.0*d)) * 0.1;
	color /= 5.0;
	FilterResult = color;
}