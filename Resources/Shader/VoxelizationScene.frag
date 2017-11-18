#version 450 core

uniform vec4 VoxelSize;
uniform vec3 SceneSizeMin;
uniform vec3 SceneSizeMax;
uniform mat4 WorldMatrix;

uniform sampler2D BaseColor ;
uniform sampler2D Roughness;
uniform sampler2D Metalic ;
uniform sampler2D Normal ;



in vec2 FSTexcoordCodrd;
in vec3 FSNormal;
in vec3 FSTangent;
in vec4 FSWorldPosition;
in vec4 FSNDCPosition;
layout(binding = 0, rgba32f)  uniform  image3D VoxelizationSceneA;
layout(binding = 1, rgba32f)  uniform  image3D VoxelizationSceneB;
layout(binding = 2, rgba32f)  uniform  image3D VoxelizationSceneC;



uniform vec3 PointLightPos ;
uniform vec3 PointLightColor ;
const float PI = 3.1415926 ;
// Lighting attenuation factors.
#define DIST_FACTOR 1.1f /* Distance is multiplied by this when calculating attenuation. */
#define CONSTANT 1
#define LINEAR 0.001
#define QUADRATIC 0
// Returns an attenuation factor given a distance.
float attenuate(float dist){ dist *= DIST_FACTOR; return 1.0f / (CONSTANT + LINEAR * dist + QUADRATIC * dist * dist); }

vec3 ComputeDiffuseColor(vec3 BaseColor , float Metalic)
{
  return BaseColor - BaseColor * Metalic;
}
vec3 ComputeSpecularColor(vec3 BaseColor , float Specular, float Metalic)
{
  return mix(vec3(Specular * 0.08) , BaseColor , Metalic);
}



//layout(location = 0) out vec4 OutColor;
void main()
{
   vec4 ABuffer , BBuffer , CBuffer;
 // 正确的情况
   // BaseColor  And Specular
   vec4  TexColor  = texture2D(BaseColor , FSTexcoordCodrd.xy) ;
   ABuffer = TexColor ;
   ABuffer.a = 0.5;
   
   //Normal Map
   //Normal
   vec3 NormalInTBN  ;
   vec3 tmpNormalInTBn  = texture2D(Normal , FSTexcoordCodrd.xy).xyz;
   //法线贴图内存储的数据格式需要转换
   NormalInTBN.x = tmpNormalInTBn.z ;
   NormalInTBN.z = tmpNormalInTBn.x ;
   NormalInTBN.y = tmpNormalInTBn.y ;
   
   
   NormalInTBN = NormalInTBN * 2.0 - vec3(1.0);
   NormalInTBN = normalize(NormalInTBN) ;
   vec3 N = normalize(FSNormal) ;
   vec3 T = normalize(FSTangent);
   vec3 B = normalize(cross(N , T));
   vec3 NormalFromNormalMap ;
   NormalFromNormalMap.x = T.x * NormalInTBN.x + B.x * NormalInTBN.y + N.x * NormalInTBN.z ;
   NormalFromNormalMap.y = T.y * NormalInTBN.x + B.y * NormalInTBN.y + N.y * NormalInTBN.z ;
   NormalFromNormalMap.z = T.z * NormalInTBN.x + B.z * NormalInTBN.y + N.z * NormalInTBN.z ;
   
   NormalFromNormalMap = (WorldMatrix * vec4(NormalFromNormalMap , 0.0)).xyz;
   BBuffer.rg = vec2(acos(NormalFromNormalMap.z) , atan(NormalFromNormalMap.y , NormalFromNormalMap.x)) ;
   

   //Roughness  And Metalic
   CBuffer.r = texture2D(Roughness , FSTexcoordCodrd.xy).r ;
   CBuffer.g = texture2D(Metalic , FSTexcoordCodrd.xy).r;

   // imageSize Return Bad Data. I Have Not Idear.
   //vec3 ImageSize = imageSize(VoxelizationScene);
   vec3 ImageSize = (SceneSizeMax - SceneSizeMin)/VoxelSize.xyz ;
   vec3 StoredPos = FSWorldPosition.xyz / ((SceneSizeMax - SceneSizeMin)/2.0);
   StoredPos = StoredPos * 0.5 + vec3(0.5);
   StoredPos *= ImageSize;
   // 这里不能使用NDC坐标去算
   //vec3 WorldSpaceNDC = FSNDCPosition.xyz ;
   //WorldSpaceNDC.z *= -1.0;
   //vec3 StoredPos = ImageSize * (WorldSpaceNDC.xyz * vec3(0.5) + vec3(0.5));
   //vec3 WorldPosition = FSNDCPosition.xyz * ((SceneSizeMax - SceneSizeMin)/2.0);



   NormalFromNormalMap = normalize(NormalFromNormalMap);
   vec3 LightDirection = PointLightPos - FSWorldPosition.xyz;
   LightDirection = normalize(LightDirection) ;
   float NoL = max(dot(NormalFromNormalMap , LightDirection) , 0.0) ;
   vec3 DiffuseColor = ComputeDiffuseColor(ABuffer.xyz, CBuffer.g);
   const float DistanceToLight = distance(FSWorldPosition.xyz, PointLightPos);
   const float SurfaceAttenuation = attenuate(DistanceToLight);
   vec3 Diffuse = DiffuseColor / PI * PointLightColor * NoL * SurfaceAttenuation;
   Diffuse = DiffuseColor / PI * PointLightColor * NoL ;
   imageStore(VoxelizationSceneA, ivec3(StoredPos), vec4(Diffuse, 1.0));


   //这里其实应该存储的是均值 但是不能读取然后相加求均值 因为在GPU中都是并行计算
   //imageStore(VoxelizationSceneA, ivec3(StoredPos), ABuffer);
   imageStore(VoxelizationSceneB, ivec3(StoredPos), BBuffer);
   imageStore(VoxelizationSceneC, ivec3(StoredPos), CBuffer);


	//if(ImageSize == vec3(0.0))
	//{
	  //OutColor.zyx = TempColor.rgb ;
	 //return;
	//}
	//OutColor.zyx = FSNDCPosition.xyz * vec3(0.5) + vec3(0.5) ;
}