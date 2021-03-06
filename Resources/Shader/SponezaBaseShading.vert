#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 Tangent;
in vec2 Texture_Coordinates ;
layout(binding = 0,shared) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreModelMatrix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
};

out vec2 TextureCoord ;
out vec3 OutNormal;
out vec3 OutTangent;
out float CameraSpaceDepth;
out vec4  ClipCoord ;
out vec4  PreClipCoord;
void main()
{
   gl_Position = ProjectMatrix * ViewMatrix * ModelMatrix * vec4(Position , 1.0);
   ClipCoord = gl_Position ;
   PreClipCoord = PreProjectMatrix * PreViewMatrix * PreModelMatrix * vec4(Position, 1.0) ;
   TextureCoord = Texture_Coordinates ;
   /*
   在Frag中计算法线贴图时候再转换 可以考虑是否可以在这里进行转化 到Frag中不再转化
   OutNormal = (ModelMatrix * vec4(Normal , 0.0)).xyz ;
   OutTangent = (ModelMatrix * vec4(Tangent.xyz , 0.0)).xyz ;
   */
   OutNormal = Normal ;
   OutTangent = Tangent.xyz ;
   
   CameraSpaceDepth = (ViewMatrix * ModelMatrix * vec4(Position , 1.0)).z;
   
}

