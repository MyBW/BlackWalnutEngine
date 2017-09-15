#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
layout(location = 0) out float DestTexture;
const vec3 Effienc = vec3(0.27, 0.67,0.06);
void main()
{
	vec3 SrcColor = texture2D(SrcTexture, textureCoord.xy).xyz;
	DestTexture = Effienc * SrcColor ;
}