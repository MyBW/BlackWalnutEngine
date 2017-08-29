#version 430 core
uniform vec4 tmpColor ;
in vec2 textureCoord ;
in vec3 normal;
in vec4 woldPosition;
uniform sampler2D tex1 ;
layout(location = 0) out vec4 BaseColorMap;
layout(location = 1) out vec4 NormalMap;
layout(location = 2) out vec4 PositionMap;
void main()
{
   vec4  TexColor  = texture2D(tex1 , textureCoord.xy) ;
   //vec4 TexColor = vec4(1.0 , 0.0 , 0.0, 0.0);
   BaseColorMap = TexColor ;
   NormalMap.rgb = normalize(normal); ;
   PositionMap.rgb = woldPosition.xyz ;
}