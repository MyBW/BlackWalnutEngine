#version 430 core

layout(location = 0) in vec3 Position;
in vec2 Texture_Coordinates ;
layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
};
out vec2 textureCoord ;
void main()
{
   gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(Position , 1.0);
   textureCoord = Texture_Coordinates ;
}