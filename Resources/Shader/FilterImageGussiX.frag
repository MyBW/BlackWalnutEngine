#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
uniform int Width ;
layout(location = 0) out vec4 FilterResult;
void main()
{
	float d = 1.0/float(Width);
    vec4 color = vec4(0.0,0.0,0.0,0.0);
    color += texture2D(SrcTexture ,textureCoord.xy+vec2(-5.0*d,0.0)) * 0.1;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2(-4.0*d,0.0)) * 0.22;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2(-3.0*d,0.0)) * 0.358;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2(-2.0*d,0.0)) * 0.523;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2(-1.0*d,0.0)) * 0.843;
    color += texture2D(SrcTexture ,textureCoord.xy ) * 1.0;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2( 1.0*d,0.0)) * 0.843;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2( 2.0*d,0.0)) * 0.523;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2( 3.0*d,0.0)) * 0.358;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2( 4.0*d,0.0)) * 0.22;
    color += texture2D(SrcTexture ,textureCoord.xy+vec2( 5.0*d,0.0)) * 0.1;
    color /= 5.0; 
    FilterResult = color;
}