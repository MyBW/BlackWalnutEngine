#version 430 core
layout(location = 0) out vec4 fragColor;
in vec2 GF_TexcoordCodrd;
void main()
{	
   fragColor.xyz = vec3(GF_TexcoordCodrd.xy, 0.0);
   fragColor.a = 1.0;
}