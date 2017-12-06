#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
in vec2 Texture_Coordinates ;
layout(binding = 0,shared) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
};
out vec2 textureCoord ;
out vec3 normal;
out vec4 woldPosition;
void main()
{
   gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(Position , 1.0);
   textureCoord = Texture_Coordinates ;
   vec4 tmpNormal ;
   tmpNormal = ModelMatrix * vec4(Normal , 0.0) ;
   normal = tmpNormal.xyz ;
   woldPosition = ModelMatrix * vec4(Position , 1.0);
}