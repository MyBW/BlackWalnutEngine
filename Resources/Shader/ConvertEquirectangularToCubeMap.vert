#version 430 core

layout(location = 0) in vec3 Position;
uniform mat4 ViewMatrix;
out vec3 WorldPosition ;
void main()
{
	WorldPosition = Position ;
    const float Width = 50; //输入的立方体模型边长是50
    float r = Width;
    float l = -Width;
    float t = Width ;
    float b = -Width ;
    float n =  - 1;
    float f1 = -Width - 1;
     mat4 OrthoProject = mat4(
      2.0 /(r -l) , 0.0, 0.0, -(r + l)/(r - l),
      0.0, 2/(t -b), 0.0, -(t + b)/(t - b),
      0.0, 0.0, 1/(f1 - n), - n/(f1 -n),
      0.0, 0.0, 0.0, 1.0
    ) ;
    mat4 ProjectMatrix = transpose(OrthoProject) ;
	gl_Position = ProjectMatrix * ViewMatrix * vec4(Position, 1.0) ;
}