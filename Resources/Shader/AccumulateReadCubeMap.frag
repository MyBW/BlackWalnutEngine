#version 430 core
in vec3 WorldPosition ;
uniform samplerCube EvnCubeMap;
uniform int MipMapLevel;
out vec4 FinalFragColor ;

void main()
{
    
	vec4 AccumulateValue = textureLod(EvnCubeMap, vec3(1.0, 0.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 1.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 0.0 ,1.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(-1.0, 0.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, -1.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 0.0 ,-1.0), MipMapLevel); 
	
	FinalFragColor =  AccumulateValue ;
}