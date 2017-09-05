#version 430 core
in vec3 WorldPosition ;
out vec4 FinalFragColor ;
uniform sampler2D AccumulatedCubeMap ;
uniform vec4 Sample01 ;
uniform vec4 Sample23;
uniform int CubeFace ;


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

float3 GetCubemapVector(float2 ScaledUVs)
{
	float3 CubeCoordinates;


	if (CubeFace == 0)
	{
		CubeCoordinates = float3(1, -ScaledUVs.y, -ScaledUVs.x);
	}
	else if (CubeFace == 1)
	{
		CubeCoordinates = float3(-1, -ScaledUVs.y, ScaledUVs.x);
	}
	else if (CubeFace == 2)
	{
		CubeCoordinates = float3(ScaledUVs.x, 1, ScaledUVs.y);
	}
	else if (CubeFace == 3)
	{
		CubeCoordinates = float3(ScaledUVs.x, -1, -ScaledUVs.y);
	}
	else if (CubeFace == 4)
	{
		CubeCoordinates = float3(ScaledUVs.x, -ScaledUVs.y, 1);
	}
	else
	{
		CubeCoordinates = float3(-ScaledUVs.x, -ScaledUVs.y, -1);
	}

	return CubeCoordinates;
}




void main()
{
    vec3 Dir = normalize(WorldPosition) ;
    vec2 UV = SampleSphericalMap(Dir) ;
    vec4 AccumlationReslut ;

	{	
	
	    float2 ScaledUVs = saturate(UV + Sample01.xy) * 2 - 1;
	    float3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += samplerCubeMap(AccumulatedCubeMap , CubeCoordinates);
	}
  
	{	
	
	    float2 ScaledUVs = saturate(UV + Sample01.zw) * 2 - 1;
	    float3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += samplerCubeMap(AccumulatedCubeMap , CubeCoordinates);
	}
	
	{	
	
	    float2 ScaledUVs = saturate(UV + Sample23.xy) * 2 - 1;
	    float3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += samplerCubeMap(AccumulatedCubeMap , CubeCoordinates);
	}
	
	{	
	
	    float2 ScaledUVs = saturate(UV + Sample23.zw) * 2 - 1;
	    float3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += samplerCubeMap(AccumulatedCubeMap , CubeCoordinates);
	}
	
	FinalFragColor = AccumlationReslut ;
}