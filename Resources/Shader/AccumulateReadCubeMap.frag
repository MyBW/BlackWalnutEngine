#version 430 core
in vec3 WorldPosition ;
uniform samplerCube EvnCubeMap;
uniform int MipMapLevel;
out vec4 FinalFragColor ;
const float PI = 3.1415926;
void main()
{
    
	vec4 AccumulateValue = textureLod(EvnCubeMap, vec3(1.0, 0.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 1.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 0.0 ,1.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(-1.0, 0.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, -1.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 0.0 ,-1.0), MipMapLevel); 
	FinalFragColor = vec4(4 * PI * AccumulateValue.rgb / ( max(AccumulateValue.a, .00001f)), 0);
}