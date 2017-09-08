#version 430 core
in vec3 WorldPosition ;
out vec4 FinalFragColor ;
uniform samplerCube AccumulatedCubeMap ;
uniform vec4 Sampler01 ;
uniform vec4 Sampler23;
uniform int CubeFace ;
uniform int MipMapLevel;


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

vec3 GetCubemapVector(vec2 ScaledUVs)
{
	vec3 CubeCoordinates;


	if (CubeFace == 0)
	{
		CubeCoordinates = vec3(1, -ScaledUVs.y, -ScaledUVs.x);
	}
	else if (CubeFace == 1)
	{
		CubeCoordinates = vec3(-1, -ScaledUVs.y, ScaledUVs.x);
	}
	else if (CubeFace == 2)
	{
		CubeCoordinates = vec3(ScaledUVs.x, 1, ScaledUVs.y);
	}
	else if (CubeFace == 3)
	{
		CubeCoordinates = vec3(ScaledUVs.x, -1, -ScaledUVs.y);
	}
	else if (CubeFace == 4)
	{
		CubeCoordinates = vec3(ScaledUVs.x, -ScaledUVs.y, 1);
	}
	else
	{
		CubeCoordinates = vec3(-ScaledUVs.x, -ScaledUVs.y, -1);
	}

	return CubeCoordinates;
}


vec2 saturate(vec2 UV)
{
  if(UV.x < 0.0) UV.x = 0.0 ;
  if(UV.y < 0.0) UV.y = 0.0 ;
  if(UV.x > 1.0) UV.x = 1.0 ;
  if(UV.y > 1.0) UV.y = 1.0 ;
  return UV;
}

void main()
{

    vec3 TmpWorldPos = WorldPosition ;
    TmpWorldPos.y *= -1;// 目前只有这样才能得到正确的cubemap 应该是引擎传入的View 矩阵有问题
    TmpWorldPos.z *= -1;
	vec3 Dir = normalize(TmpWorldPos);
    vec2 UV = SampleSphericalMap(Dir) ;
    vec4 AccumlationReslut = vec4(0.0) ;

	{	
	
	    vec2 ScaledUVs = saturate(UV + Sampler01.xy) * 2 - 1;
	    vec3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += textureLod(AccumulatedCubeMap , CubeCoordinates, MipMapLevel);
		//AccumlationReslut += textureLod(AccumulatedCubeMap , CubeCoordinates, MipMapLevel);
	}
  
	{	
	
	    vec2 ScaledUVs = saturate(UV + Sampler01.zw) * 2 - 1;
	    vec3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += textureLod(AccumulatedCubeMap , CubeCoordinates, MipMapLevel);
	}
	
	{	
	
	    vec2 ScaledUVs = saturate(UV + Sampler23.xy) * 2 - 1;
	    vec3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += textureLod(AccumulatedCubeMap , CubeCoordinates, MipMapLevel);
	}
	
	{	
	
	    vec2 ScaledUVs = saturate(UV + Sampler23.zw) * 2 - 1;
	    vec3 CubeCoordinates = GetCubemapVector(ScaledUVs);
	    AccumlationReslut += textureLod(AccumulatedCubeMap , CubeCoordinates, MipMapLevel);
	}
	
	FinalFragColor = AccumlationReslut/4.0 ;
}