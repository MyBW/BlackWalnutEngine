#version 430 core
uniform sampler2D BaseColorMap ;
uniform sampler2D NormalMap ;
uniform sampler2D PositionMap;
uniform vec3 PointLightPosition; 
uniform vec2 gScreenSize ;
void main()
{
    
    vec2 Text2d = gl_FragCoord.xy / gScreenSize;
    vec4  baseColor  = texture2D(BaseColorMap, Text2d.xy) ;
    vec4  normal = texture2D(NormalMap, Text2d.xy) ;
	vec4  position = texture2D(PositionMap , Text2d.xy);
    vec4 LightDir = vec4(1.0 , 1.0 , 1.0 , 0.0);
    LightDir.xyz = PointLightPosition - position.xyz;
	LightDir = normalize(LightDir) ;
	baseColor.a = 1.0f ;// 后期混合要使用 这里理解一下
    baseColor *= dot(normal, LightDir) ;
	baseColor.r = 1.0 ;
    gl_FragColor = baseColor ;
	//gl_FragColor = vec4(1.0 , 1.0 , 0.0 , 1.0);
}