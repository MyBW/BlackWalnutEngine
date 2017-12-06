#version 430 core
layout(location = 0) in vec3 Position;
in vec2 Texture_Coordinates ;
out vec2 textureCoord ;
uniform mat4  ModelMatrix;
uniform mat4  PreModelMatrix;
layout(binding = 0,shared) uniform ViewportInformation
{
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
  mat4  ViewInversMatrix;
  vec3  ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2  NearFar;
  vec2  ScreenWH ;
};

void main()
{
    const float Width = 50; //输入的立方体模型边长是50
    float r = Width;
    float l = -Width;
    float t = Width ;
    float b = -Width ;
    float n = -200;
    float f = 200;
    mat4 OrthoProject = mat4(
      2.0 /(r -l) , 0.0, 0.0, -(r + l)/(r - l),
      0.0, 2/(t -b), 0.0, -(t + b)/(t - b),
      0.0, 0.0, 1/(f - n), - n/(f -n),
      0.0, 0.0, 0.0, 1.0
    ) ;
    mat4 ProjectMatrix = transpose(OrthoProject) ;
    textureCoord = Texture_Coordinates ;
    if(textureCoord.y == 0) textureCoord.y = 1 ;
    else textureCoord.y = 0 ;
    gl_Position = ProjectMatrix * vec4(Position , 1.0);
}