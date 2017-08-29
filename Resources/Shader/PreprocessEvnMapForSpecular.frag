#version 430 core
/*float RadicalInverse(int Base , int i)
{
   float Digit , Radical , Inverse ;
   float DBase = Base ;
   Digit = Radical = 1.0/ DBase ;
   Inverse = 0.0 ;
   while(i != 0)
   {
      Inverse += Digit * (i % Base) ;
    Digit *= Radical ;
    i /= Base ;
   }
   return Inverse ;
}

float Hammersley(int Dimension , int Index , int NumberSamples)
{
    if(Dimension == 0)
  {
     float TmpNumberSamples = NumberSamples ;
     return  Index / TmpNumberSamples ;
  }
  else
  {
     return RadicalInverse(2 , Index) ;
  }
}*/

float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
  float a = roughness*roughness;
  float PI = 3.1415926;
  float phi = 2.0 * PI * Xi.x;
  float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
  float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
  
  // from spherical coordinates to cartesian coordinates - halfway vector
  vec3 H;
  H.x = cos(phi) * sinTheta;
  H.y = sin(phi) * sinTheta;
  H.z = cosTheta;
  
  // from tangent-space H vector to world-space sample vector
  vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
  vec3 tangent   = normalize(cross(up, N));
  vec3 bitangent = cross(N, tangent);
  
  vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
  return normalize(sampleVec);
}


in vec3 WorldPosition ;
uniform float roughness ;
uniform samplerCube EvnMap ;
layout(location = 0) out vec4 FinalColor ;
void main()
{
    //这里要区别对待处理环境贴图漫发射部分的Noraml，漫反射中的Normal表示的方向，在实时计算中表示的意思是
    //物体表面法线和Noraml方向一致时，可以接受的辐射强度radiance，然后使用kd *Diff/PI*radiance来计算环境
    //贴图对物体漫反射光照的影响
    //但是这里处理环境光照的镜面反射时，Normal的意思是在实时渲染中视线的反射方向
    vec3 TmpWorldPos = WorldPosition ;
    TmpWorldPos.y *= -1;// 目前只有这样才能得到正确的cubemap 应该是引擎传入的View 矩阵有问题
    TmpWorldPos.z *= -1;
    vec3 Normal = normalize(TmpWorldPos) ;
    vec3 R = Normal ;
    vec3 V = R ;

    float Roughness = roughness;
    int SampleNumber  = 4096 ;
    vec3 Sum = vec3(0.0) ;
    float SumWidght = 0.0 ;
    for (int i = 0; i < SampleNumber; ++i)
    {
        vec2 Xi = Hammersley(i , SampleNumber) ;
        vec3 H = ImportanceSampleGGX(Xi , Normal , Roughness) ; //一般都是对物理BRDF中的D项进行重要性采样，这里的GGX是 cos 相关的函数
        vec3 L = normalize(2.0*dot(H , V) * H - V) ;

        float NoL = dot(Normal , L) ;
        if (NoL > 0.0f)
        {
            Sum += texture(EvnMap , L).rgb * NoL ; //GGX是cos相关的函数 所以要用NoL
            SumWidght += NoL ;
        }
    }
    FinalColor = vec4(Sum / SumWidght ,1.0) ;
    //FinalColor = vec4(texture(EvnMap , Normal).rgb ,1.0) ;
    //gl_FragColor = vec4(texture(EvnMap , Normal).rgb ,1.0) ;
    //gl_FragColor = vec4(1.0 ,0.0 ,0.0 ,1.0) ;
    //gl_FragColor = vec4(Sum / SumWidght ,1.0) ;
}


