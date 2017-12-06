#version 430 core

layout(location = 0) in vec3 Position;
uniform mat4 tmpModelMatrix ;
layout(binding = 0,shared) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
};
void main()
{
   //gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(Position , 1.0);
   gl_Position = ProjectMatrix * ViewMatrix * tmpModelMatrix * vec4(Position , 1.0);
}