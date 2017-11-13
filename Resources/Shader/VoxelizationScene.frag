#version 450 core

uniform vec4 VoxelSize;
uniform vec3 SceneSizeMin;
uniform vec3 SceneSizeMax;

in vec2 FSTexcoordCodrd;
in vec3 FSNormal;
in vec3 FSTangent;
in vec4 FSWorldPosition;
in vec4 FSNDCPosition;
layout(binding = 0, rgba32f)  uniform  image3D VoxelizationScene;
//layout(location = 0) out vec4 OutColor;
void main()
{
    // imageSize Return Bad Data. I Have Not Idear.
	//vec3 ImageSize = imageSize(VoxelizationScene);
 
	vec3 ImageSize = (SceneSizeMax - SceneSizeMin)/VoxelSize.xyz ;
	vec3 StoredPos = ImageSize * (FSNDCPosition.xyz * vec3(0.5) + vec3(0.5));
	vec4 TempColor = vec4(1.0 , 0.0, 0.0, 0.0) ;
	imageStore(VoxelizationScene, ivec3(StoredPos), TempColor);
	//if(ImageSize == vec3(0.0))
	//{
	  //OutColor.zyx = TempColor.rgb ;
	 //return;
	//}
	//OutColor.zyx = FSNDCPosition.xyz * vec3(0.5) + vec3(0.5) ;
}