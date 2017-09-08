#version 430 core

in vec3 textureCoord ;
uniform samplerCube CubeMapTexture;

void main()
{
   gl_FragColor = texture(CubeMapTexture, textureCoord);
}