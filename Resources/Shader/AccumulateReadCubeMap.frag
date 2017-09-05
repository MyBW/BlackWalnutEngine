#version 430 core
in vec3 WorldPosition ;
uniform samplerCube EvnCubeMap;
out vec4 FinalFragColor ;
uniform int MipMapLevel;
void main()
{
    
	float4 AccumulateValue = textureLod(EvnCubeMap, vec3(1.0, 0.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 1.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 0.0 ,1.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(-1.0, 0.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, -1.0 ,0.0), MipMapLevel); 
	AccumulateValue += textureLod(EvnCubeMap, vec3(0.0, 0.0 ,-1.0), MipMapLevel); 
	
	FinalFragColor =  AccumulateValue ;
}