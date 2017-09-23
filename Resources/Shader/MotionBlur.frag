#version 430 core
in vec2 textureCoord ;
uniform sampler2D FinalReslutImage ;
uniform sampler2D VeclocityRT ;
layout(location = 0) out vec4 FilterResult;
void main()
{
	vec2 VeclocityVec = texture2D(VeclocityRT, textureCoord.xy).xy / 2.0 ;
	vec2 TextureCoord = textureCoord.xy;
	
	FilterResult = texture2D(FinalReslutImage , textureCoord.xy) * 0.4 ;
	
	TextureCoord -= VeclocityVec ;
	FilterResult += texture2D(FinalReslutImage , TextureCoord) * 0.3 ;
	
	TextureCoord -= VeclocityVec ;
	FilterResult += texture2D(FinalReslutImage , TextureCoord) * 0.2 ;
	
	TextureCoord -= VeclocityVec ;
	FilterResult += texture2D(FinalReslutImage , TextureCoord) * 0.1 ;
	
}