
#version 430 core
in vec2 textureCoord ;
uniform sampler2D BBuffer;
uniform sampler2D FinalRenderResult;

uniform float StrideZCutoff;
uniform float Thickness;
uniform float MaxDistance;
uniform float StrideInPixel; // > 1
uniform float MaxStepCount;


layout(location = 0) out vec4 FinalFilterResult;
layout(binding = 0,std140) uniform UBO1
{
  mat4  ModelMatrix;
  mat4  ViewMatrix;
  mat4  ProjectMatrix;
  mat4  PreModelMatix;
  mat4  PreViewMatrix;
  mat4  PreProjectMatrix;
};
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};

vec4 FromScreenToWorld(mat4 InViewInverse , vec2 InScreenPos , vec2 InScreenWH, float InFov , vec2 InNearFar , float InPrjPlaneWInverseH,float InCameraDepth)
{
   vec2 Pos;
   Pos.xy = (vec2(1.0) - textureCoord.xy )* vec2(2.0) - vec2(1.0);
   vec4 CameraSpacePos;
   CameraSpacePos.x = Pos.x * InPrjPlaneWInverseH * tan(InFov/2.0f) * InCameraDepth;
   CameraSpacePos.y = Pos.y * tan(InFov / 2.0f) * InCameraDepth;
   CameraSpacePos.z = InCameraDepth;
   CameraSpacePos.w = 1.0f;
   return InViewInverse * CameraSpacePos;
}
vec3 ComputerNormal(vec2 InNormalXY)
{
   vec3 Normal ;
   float sinThta = sin(InNormalXY.x) ;
   float cosThta = cos(InNormalXY.x) ;
   float sinPh = sin(InNormalXY.y ) ;
   float cosPh = cos(InNormalXY.y ) ;
   Normal.x = sinThta * cosPh ;
   Normal.y = sinThta * sinPh ;
   Normal.z = cosThta ;
   return Normal;
}
// 注意这里的所有Z都是负数
bool IntersectsDepthBuffer(float z, float minZ , float maxZ)
{
	float depthScale = min(1.0f, -z * StrideZCutoff);// 距离摄像机越远 DepthScale 就越大
	z -= Thickness + 0.0 + (2.0 - 0.0)*depthScale;
	return (maxZ <= z) &&( minZ  > z);
}
float LinearDepthTexelFetch(vec2 UV)
{
  float CameraSpaceDepth = texelFetch(BBuffer, ivec2(UV), 0).z; 
  return CameraSpaceDepth ;
  //UV = UV/ScreenWH ;
  //if(UV. x > 1.0f || UV.y > 1.0f || UV.x < 0.0f || UV.y < 0.0f)
  //return 0;
  //float CameraSpaceDepth = texture2D(BBuffer , UV).z;
  //return CameraSpaceDepth;
  //float TempA = (NearFar.x + NearFar.y)/(NearFar.y - NearFar.x);
  //float TempB = (2 * NearFar.x * NearFar.y )/(NearFar.x - NearFar.y);
  //return (CameraSpaceDepth * TempA + TempB)/CameraSpaceDepth; // NDC Z
}
float DistanceSquared(vec2 A , vec2 B)
{
  A -= B;
  return dot(A, A);
}
void Swap(out float A,out float B)
{
	float C = A ;
	A = B;
	B = C ;
}

//View Space
bool GetIntersectionPoint(vec3 OriginalPoint , vec3 Direction, float Jitter ,out vec3 IntersectionPos , out vec2 HitPixel)
{
	float RayLenth = (OriginalPoint.z + Direction.z * MaxDistance) > (-NearFar.x )? (-NearFar.x - OriginalPoint.z)/ Direction.z :MaxDistance;
	//RayLenth = 10000 ;
	vec3 EndPoint = OriginalPoint + Direction * RayLenth;
	
	vec4 HS = ProjectMatrix * vec4(OriginalPoint, 1.0);
	vec4 HE = ProjectMatrix * vec4(EndPoint, 1.0);
	float KS = 1.0f / HS.w;
	float KE = 1.0f / HE.w;
	// homogeneours pos of camera version
	vec3 QS = OriginalPoint * KS;
	vec3 QE = EndPoint * KE;
	
	//Pixel Pos
	vec2 PS = (HS.xy * KS * vec2(0.5) + vec2(0.5)) * ScreenWH ;
	vec2 PE = (HE.xy * KE * vec2(0.5) + vec2(0.5)) * ScreenWH ;
	
	 
	PE += (DistanceSquared(PS, PE) < 0.00001f) ? vec2(0.02f ,0.02f) : vec2(0.0f);
	vec2 Delta = PE - PS;
	bool Permute = false;
	if(abs(Delta.x) < abs(Delta.y))
	{
	  Permute = true;
	  Delta = Delta.yx ;
	  PS = PS.yx;
	  PE = PE.yx;
	}
	float StepDir = sign(Delta.x);
	float invDx = StepDir / Delta.x;
	
	vec3 dQ = (QE - QS) * invDx;
	float dK = (KE - KS) * invDx;
	vec2  dP = vec2(StepDir, Delta.y * invDx);
	
	float strideScale = 1.0f - min(1.0f, -OriginalPoint.z * StrideZCutoff);
    float stride = 1.0f + strideScale * StrideInPixel;
    dP *= stride;
    dQ *= stride;
    dK *= stride;
	
	PS += dP * Jitter;
    QS += dQ * Jitter;
    KS += dK * Jitter;
	
	vec4 PQK = vec4(PS, QS.z ,KS);
	vec4 dPQK = vec4(dP, dQ.z, dK);
	vec3 Q = QS;
	
	float End = PE.x * StepDir;
	float Count = 0.0f;
	float PrevZMaxEstimate = OriginalPoint.z;
	float RayZMin = PrevZMaxEstimate;
    float RayZMax = PrevZMaxEstimate;
    float SceneZMax = RayZMax + 1e4;
    
	bool IsHit = false;
    for(;(((PQK.x * StepDir) < End)&&(Count < MaxStepCount)&& !IsHit);Count++)
    {
	    PQK += dPQK;
		HitPixel = Permute ? PQK.yx : PQK.xy;
		
		SceneZMax = LinearDepthTexelFetch(HitPixel);
		RayZMin = PrevZMaxEstimate;
		RayZMax = (dPQK.z *0.5f + PQK.z)/(dPQK.w * 0.5f + PQK.w);
		PrevZMaxEstimate = RayZMax;
		
		if(RayZMin < RayZMax)
		{
		  Swap(RayZMin , RayZMax);
		}
		
		//RayZMin += Thickness ;
		//RayZMax -= Thickness;
		
		float depthScale = min(1.0f, -SceneZMax * StrideZCutoff);// 距离摄像机越远 DepthScale 就越大
	    SceneZMax -= Thickness + 0.0 + (2.0 - 0.0)*depthScale;
	    
		
		if( SceneZMax == 0) break;
		IsHit = (RayZMax <= SceneZMax && RayZMin > SceneZMax);
	}
	return IsHit;
	//Q.xy += dQ.xy * Count;
	//IntersectionPos = Q * (1.0f /PQK.w);
	//if(Count > 100) return true;
	//return false;
	//return ((RayZMax < (SceneZMax - 0.001)) && (RayZMin > SceneZMax));
	//return IntersectsDepthBuffer(SceneZMax, RayZMin, RayZMax);
}

void GetIntersectionPointByTest(vec4 WorldPos, vec3 R, out vec3 FinalColor)
{
    float StepSize = 100.0;
    FinalColor = vec3(0.0);
	vec4 Intersection = vec4(WorldPos.xyz , 1.0);
	Intersection.w = 1.0 ;	
	int count  = 100;
    while(count > 0)
	{
	    count--;
	    Intersection.xyz = Intersection.xyz + R * StepSize ;
	    vec4 ProcessIntersection = Intersection ;
	    float IntersectionCameraDepth = ProcessIntersection.z ;
		ProcessIntersection = ProjectMatrix *  ProcessIntersection ;
		ProcessIntersection.xy = ProcessIntersection.xy / ProcessIntersection.w ;
		ProcessIntersection.xy = ProcessIntersection.xy * 0.5 + vec2(0.5) ;
		
		if(ProcessIntersection.x < 0.0 || ProcessIntersection.x > 1.0 || ProcessIntersection.y > 1.0 || ProcessIntersection.y < 0.0)
		{
		  Intersection.xyz = Intersection.xyz - R * StepSize ;
		  StepSize /= 2.0 ;
		  if(StepSize < 0.0000001)
		  {
		    break;
		  }
		  continue ;
		}
		float CurrentDepth = texture2D(BBuffer,ProcessIntersection.xy).z ;
		if(CurrentDepth < IntersectionCameraDepth)
		continue ;
		if(CurrentDepth > IntersectionCameraDepth)
		{
		  Intersection.xyz = Intersection.xyz - R * StepSize ;
		  StepSize /= 2.0 ;
		  if(StepSize < 0.0000001)
		  break;
		  continue;
		}
	    FinalColor = texture2D(FinalRenderResult, ProcessIntersection.xy).rgb;
		break;
	}
}
void main()
{	
	vec2 PixelStepSize = vec2(1.0f /ScreenWH.x , 1.0f / ScreenWH.y);
	vec4 BBufferData = texture2D(BBuffer, textureCoord.xy) ;
	float CameraSpaceDepth = BBufferData.z ;
	vec2 NF = vec2( NearFar.x , NearFar.y) ;
	NF.x =1;
	NF.y = 600;
	float NewFoV = 3.1415926/2.0;
	vec2 NewScreenWH = vec2(1024 , 768);
	float NewPrjPlaneWInverseH = 1.33333333;
	
    vec4 WorldPos = FromScreenToWorld(ViewInversMatrix , gl_FragCoord.xy, NewScreenWH, NewFoV, NF, NewPrjPlaneWInverseH, CameraSpaceDepth) ;	
    vec3  Normal = ComputerNormal(vec2(BBufferData.xy)) ;
    Normal = normalize(Normal) ;
    vec3 ViewDirection = normalize(ViewPositionWorldSpace - WorldPos.xyz);
    vec3 R = reflect(-ViewDirection , Normal) ;
    R = normalize(R);
	
	vec3 FinalColor = vec3(0.0f);
	
	vec3 IntersectionPosition;
	vec2 HitPixelPos;
	vec4 OriginalPoint = ViewMatrix * WorldPos;
	vec4 Direction = ViewMatrix * vec4(R , 0.0);
	Direction = normalize(Direction);
	
	float Jitter = StrideInPixel > 1.0f ? float(int(textureCoord.x * ScreenWH.x + textureCoord.y * ScreenWH.y) & 1) * 0.5f : 0.0f;

	bool IsHitPixel = false;
	IsHitPixel = GetIntersectionPoint(OriginalPoint.xyz , Direction.xyz, Jitter ,IntersectionPosition ,HitPixelPos);
	if(IsHitPixel)
	{
	  HitPixelPos /= ScreenWH ;
	  //HitPixelPos = HitPixelPos * vec2(0.5) + vec2(0.5);
	  if(HitPixelPos.x < 0.0f || HitPixelPos.y < 0.0f || HitPixelPos.x > 1.0f || HitPixelPos.y > 1.0f)
	  {
	    FinalColor = vec3(0.0);
	  }
	  else
	  {
	    FinalColor  = texture2D(FinalRenderResult, HitPixelPos.xy).rgb;
	  }
	  //FinalColor.r = 1.0;
	  //FinalColor = texture2D(FinalRenderResult, HitPixelPos.xy).rgb;  
	}
	
	//GetIntersectionPointByTest(OriginalPoint, Direction.xyz, FinalColor);
    FinalFilterResult.rgb = 0.2 * texture2D(FinalRenderResult, textureCoord.xy).rgb + FinalColor ;
	//FinalFilterResult.rgb = FinalColor;
	FinalFilterResult.w = 1.0;
}