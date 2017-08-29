#version 430 core
uniform vec4 tmpColor ;
in vec2 textureCoord ;
uniform sampler2D tex1 ;
void main()
{
   vec4  color  = texture2D(tex1 , textureCoord.xy) ;
   //color = color + tmpColor ;
   gl_FragColor = color ;	
	//gl_FragColor = vec4(1.0 ,0.0 ,0.0 ,0.0) ;
}