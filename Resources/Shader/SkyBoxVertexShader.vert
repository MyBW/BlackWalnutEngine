#version 430 core

layout(location = 0) in vec3 Position;
layout(binding = 0,shared) uniform UBO1
{
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  vec3  CameraPosition;
};
out vec3 textureCoord ;
void main()
{
    mat4 CameraModelMatrix = 
    mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        CameraPosition.x, CameraPosition.y, CameraPosition.z, 1
    );
    vec4 WVP_Pos = ProjectMatrix * ViewMatrix * CameraModelMatrix * vec4(Position, 1.0);
    gl_Position = WVP_Pos.xyww; 
    textureCoord = Position;
}