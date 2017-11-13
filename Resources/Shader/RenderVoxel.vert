#version 450

layout(location = 0) in vec3 WorldPosition;
layout(location = 1) in vec3 TextureCoord;

out vec3 GSTextureCoord;
void main()
{
		GSTextureCoord = TextureCoord;
		gl_Position = vec4(WorldPosition, 1.0);
}