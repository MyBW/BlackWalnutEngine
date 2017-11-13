#version 450 core

layout(location = 0 ) out vec4 FinalColor;

in vec3 FSTextureCoord;
uniform sampler3D VoxelScene;
void main()
{
	vec4 SampleColor = textureLod(VoxelScene, FSTextureCoord, 0);
	if(SampleColor == vec4(0.0))
	{
	   //SampleColor = vec4(0, 1.0, 0, 0); // Debug Code
	   discard;
	}

    //FinalColor.xyz = FSTextureCoord;
    FinalColor = SampleColor;
}