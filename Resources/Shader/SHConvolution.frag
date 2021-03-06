#version 430 core
in vec3 WorldPosition ;
uniform samplerCube EvnCubeMap ;
uniform vec4 Mask0;
uniform vec4 Mask1;
uniform float Mask2;
out vec4 FinalFragColor ;

const vec2 invAtan = vec2(0.1591, 0.3183);
struct FThreeBandSHVector
{
	vec4  V0;
	vec4  V1;
	float  V2;
};
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}


FThreeBandSHVector SHBasisFunction3( vec3  InputVector)
{
	FThreeBandSHVector Result;

	Result.V0.x = 0.282095f;
	Result.V0.y = -0.488603f * InputVector.y;
	Result.V0.z = 0.488603f * InputVector.z;
	Result.V0.w = -0.488603f * InputVector.x;

	vec3  VectorSquared = InputVector * InputVector;
	Result.V1.x = 1.092548f * InputVector.x * InputVector.y;
	Result.V1.y = -1.092548f * InputVector.y * InputVector.z;
	Result.V1.z = 0.315392f * (3.0f * VectorSquared.z - 1.0f);
	Result.V1.w = -1.092548f * InputVector.x * InputVector.z;
	Result.V2 = 0.546274f * (VectorSquared.x - VectorSquared.y);

	return Result;
}

void main()
{
    vec3 TmpWorldPos = WorldPosition ;
    TmpWorldPos.y *= -1;// 目前只有这样才能得到正确的cubemap 应该是引擎传入的View 矩阵有问题
    TmpWorldPos.z *= -1;
	vec3 Dir = normalize(TmpWorldPos);
	
	
	vec3 Lighting = texture(EvnCubeMap, Dir).xyz;
	
    vec2 ScaledUVs = SampleSphericalMap(Dir) ;
	float SquaredUVs = 1 + dot(ScaledUVs, ScaledUVs);
	float TexelWeight = 4 / (sqrt(SquaredUVs) * SquaredUVs);

    FThreeBandSHVector SHCoefficients = SHBasisFunction3(Dir);
    
	float SHCofficient = dot(SHCoefficients.V0 , Mask0) + dot(SHCoefficients.V1, Mask1) + SHCoefficients.V2 * Mask2 ;
    FinalFragColor = vec4(Lighting * SHCofficient * TexelWeight, TexelWeight);
	//FinalFragColor = vec4(Lighting,1.0);
}