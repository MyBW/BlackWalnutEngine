#version 430 core

in vec3 WorldPosition ;
uniform samplerCube EvnMap;
out vec4 Front;
void main()
{
    vec3 TmpWorldPos = WorldPosition ;
    TmpWorldPos.y *= -1;// 目前只有这样才能得到正确的cubemap 应该是引擎传入的View 矩阵有问题
    TmpWorldPos.z *= -1;
	vec3 Normal = normalize(TmpWorldPos) ;
    vec3 Up = vec3(0.0 ,1.0 ,0.0) ;
    vec3 Right = cross(Up , Normal) ;
    Up = cross(Normal , Right) ;
	float Theta = 0.0 ;
	float Alpha = 0.0 ;
	const int SampleNumber = 1024 ;
	const float PI = 3.1415926 ;
	float ThetaStep =  2 * PI / SampleNumber ;
	float AlphaStep =  0.5 * PI / SampleNumber ;
	int AllSampleNumber = 0 ;
	vec3  Sum = vec3(0.0) ;
	for(  ; Theta < 2 * PI  ; Theta += ThetaStep)
	{
		for(; Alpha < 0.5 * PI ; Alpha += AlphaStep)
		{
		    vec3 Dir ;
			Dir.x = sin(Alpha) * cos(Theta) ;
			Dir.y = sin(Alpha) * sin(Theta) ;
			Dir.z = cos(Alpha) ;
            Dir = Right * Dir.x + Up * Dir.y + Normal* Dir.z ;
            vec3 Irradiance = texture(EvnMap , Dir).xyz ;
            Sum += Irradiance * cos(Alpha) * sin(Alpha) ;
            AllSampleNumber++ ;
		}
	}
     //vec3 FinalColor = texture(EvnMap , TmpWorldPos).xyz;
     vec3 FinalColor= PI * Sum * 1.0 /float(AllSampleNumber); //这里有Pi的原因是均匀采样要求的面积
     Front = vec4(FinalColor ,1.0);
}