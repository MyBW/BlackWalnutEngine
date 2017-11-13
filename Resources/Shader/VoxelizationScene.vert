#version 450 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 Tangent;
in vec2 Texture_Coordinates ;

uniform mat4 WorldMatrix;
uniform vec3 SceneSizeMin;
uniform vec3 SceneSizeMax;


out vec2 GSTextureCoord ;
out vec3 GSNormal;
out vec3 GSTangent;
out vec4 GSWorldPostion ;
void main()
{
    float r = SceneSizeMin.x;
    float l = SceneSizeMax.x;
    float t = SceneSizeMax.y;
    float b = SceneSizeMin.y ;
    float n = -SceneSizeMin.z;
    float f = -SceneSizeMax.z;
    

	mat4 OrthoProject = mat4(
      2.0 /(r -l) , 0.0, 0.0, -(r + l)/(r - l),
      0.0, 2/(t -b), 0.0, -(t + b)/(t - b),
      0.0, 0.0, -2.0/(f - n), - (f + n)/(f -n),
      0.0, 0.0, 0.0, 1.0
    ) ;
   mat4 ProjectMatrix = transpose(OrthoProject) ;

    gl_Position = ProjectMatrix * WorldMatrix * vec4(Position, 1.0) ;
    GSTextureCoord = Texture_Coordinates;
    GSWorldPostion = WorldMatrix * vec4(Position, 1.0) ;
    GSNormal = (WorldMatrix * vec4(Normal , 0.0)).xyz;
    GSTangent = (WorldMatrix * vec4(Tangent, 0.0)).xyz;

}
