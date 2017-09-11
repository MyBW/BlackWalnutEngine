#version 430 core
in vec2 textureCoord ;
uniform sampler2D AmbientOcclusion ;
layout(binding = 0,std140) uniform CameraInfo
{
  mat4 ViewInversMatrix;
  vec3 ViewPositionWorldSpace;
  float FoV ;
  float PrjPlaneWInverseH;
  vec2 NearFar;
  vec2 ScreenWH ;
};
layout(location = 0) out vec4 Attenuation ;
void main()
{
    float StepX = 1.0 / ScreenWH.x ;
	float StepY = 1.0 / ScreenWH.y ;
	float FinalValue = 0.0;
	for(int i = -2 ; i <= 2; i++)
	 for(int j = -2; j <= 2 ; j++)
	 {
		vec2 Sample = textureCoord + vec2( i * StepX , j * StepY) ;
		FinalValue += texture2D(AmbientOcclusion , Sample).w ;
	 )
	Attenuation.w = FinalValue/ 16 ;
}
