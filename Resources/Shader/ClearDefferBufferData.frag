#version 430 core
in vec2 TextureCoord ;
in vec3 OutNormal;
in vec3 OutTangent ;
in float CameraSpaceDepth;
in vec4  ClipCoord ;
in vec4  PreClipCoord;

layout(location = 0) out vec4 ABuffer;
layout(location = 1) out vec4 BBuffer;
layout(location = 2) out vec4 CBuffer;
layout(location = 3) out vec4 VelocityRT;

void main()
{
   ABuffer = vec4(0.0f);
   BBuffer = vec4(0.0f);
   CBuffer = vec4(0.0f);
   VelocityRT = vec4(0.0f);
}