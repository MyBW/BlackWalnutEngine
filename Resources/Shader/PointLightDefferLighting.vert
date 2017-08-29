#version 430 core

layout(location = 0) in vec3 Position;
uniform float RadiusScale;
uniform mat4  tmpModelMatrix ;
layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
};
out mat4 modelMatrix ;
void main()
{
    //gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(Position * RadiusScale, 1.0);
    gl_Position = ProjectMatrix * ViewMatrix * tmpModelMatrix * vec4(Position * RadiusScale, 1.0);
}