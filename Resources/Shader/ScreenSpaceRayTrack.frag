
#version 430 core
in vec2 textureCoord ;
uniform sampler2D BBuffer;
uniform sampler2D FinalRenderResult;

uniform float StrideZCutoff;
uniform float Thickness;
uniform float MaxDistance;
uniform float StrideInPixel; // > 1
uniform float MaxStepCount;


layout(location = 0) out vec4 HitInformation;
layout(binding = 1,std140) uniform UBO1
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
bool GetIntersectionPoint(vec3 OriginalPoint , vec3 Direction, float Jitter ,out vec3 IntersectionPosInfo , out vec2 HitPixel)
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
		
		RayZMin += Thickness ;
		RayZMax -= Thickness;
		float depthScale = min(1.0f, -SceneZMax * StrideZCutoff);
	    SceneZMax -= Thickness + 0.0 + (2.0 - 0.0)*depthScale;
		IsHit = (RayZMax <= SceneZMax && RayZMin > SceneZMax);
	}
	if(IsHit) 
	{
	  IntersectionPosInfo = texelFetch(BBuffer, ivec2(HitPixel), 0).xyz;
	  if(IntersectionPosInfo == vec3(0.0))  // Hit The Sky Box
	  {
	     IsHit = false;
	  }
	}
	return IsHit;
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

vec4 FromScreenToCamera(vec2 ClipSpaceXY ,float InFov , float InPrjPlaneWInverseH,float InCameraDepth)
{
   vec2 Pos;
   Pos.xy = ClipSpaceXY;
   vec4 CameraSpacePos;
   CameraSpacePos.x = Pos.x * InPrjPlaneWInverseH * tan(InFov/2.0f) * InCameraDepth;
   CameraSpacePos.y = Pos.y * tan(InFov / 2.0f) * InCameraDepth;
   CameraSpacePos.z = InCameraDepth;
   CameraSpacePos.w = 1.0f;
   return CameraSpacePos;
}

void main()
{	
	vec4 BBufferData = texture2D(BBuffer, textureCoord.xy) ;
	float CameraSpaceDepth = BBufferData.z ;
	if(BBufferData.xyz == vec3(0.0)) // GBuffer Bad Data  (Sky Box Area)
	{
	  HitInformation = vec4(0.0, 0.0, 0.0, 0.0);
	  return;
	}
	vec2 ClipXY = (vec2(1.0) - textureCoord.xy) * 2.0 - vec2(1.0);
    vec4 CameraSpacePos = FromScreenToCamera(ClipXY, FoV, PrjPlaneWInverseH, CameraSpaceDepth);
    vec3  Normal = ComputerNormal(vec2(BBufferData.xy)) ;
    Normal = (ViewMatrix * vec4(Normal, 0.0)).xyz;
    Normal = normalize(Normal) ;
    vec3 ViewDirection = normalize(CameraSpacePos.xyz);
    vec3 R = reflect(ViewDirection , Normal) ;
    R = normalize(R);
	
	
	vec3 IntersectionPositionInfo;
	vec2 HitPixelPos;
	vec4 OriginalPoint = CameraSpacePos;
	float Jitter = StrideInPixel > 1.0f ? float(int(textureCoord.x * ScreenWH.x + textureCoord.y * ScreenWH.y) & 1) * 0.5f : 0.0f;
	bool IsHitPixel = false;
	IsHitPixel = GetIntersectionPoint(OriginalPoint.xyz , R, Jitter ,IntersectionPositionInfo ,HitPixelPos);


	//Debug Code
	//vec3 FinalColor = vec3(0.0f);
	//if(IsHitPixel)
	//{
	//  HitPixelPos /= ScreenWH;
	//  FinalColor  = texture2D(FinalRenderResult, HitPixelPos.xy).rgb;
	//}
	//else
	//{
	//  HitPixelPos = vec2(0.0);
	//}
	// HitInformation.xyz = FinalColor;

    HitInformation.xyz = vec3(0.0);
	if(IsHitPixel)
	{
	  HitInformation.xy = HitPixelPos/ScreenWH;
	  HitInformation.z = IntersectionPositionInfo.z ;	
	}
}