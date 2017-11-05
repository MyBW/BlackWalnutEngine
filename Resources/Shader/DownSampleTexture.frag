#version 430 core
in vec2 textureCoord ;
uniform sampler2D SrcTexture ;
uniform int Width ;
uniform int Hight;
uniform int Miplevel;
layout(location = 0) out vec4 DestTexture;



//ScaleCopy.fs 用于下采样图象到1/4大小
vec4 GetScaleFourValue()
{
    float dx = 1.0/float(Width);
    float dy = 1.0/float(Hight);
    vec4 color = textureLod(SrcTexture, textureCoord.xy, Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx,0.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*2.0,0.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*3.0,0.0), Miplevel);
                       
    color += textureLod(SrcTexture, textureCoord.xy+vec2(0.0,dy), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx,dy), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*2.0,dy), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*3.0,dy), Miplevel);
                       
    color += textureLod(SrcTexture, textureCoord.xy+vec2(0.0,dy*2.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx,dy*2.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*2.0,dy*2.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*3.0,dy*2.0), Miplevel);
                       
    color += textureLod(SrcTexture, textureCoord.xy+vec2(0.0,dy*3.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx,dy*3.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*2.0,dy*3.0), Miplevel);
    color += textureLod(SrcTexture, textureCoord.xy+vec2(dx*3.0,dy*3.0), Miplevel);

    color /= 16.0;
    return color ;
}


//DownSample8.htm 下采样到1/8大小，可能丢失细节。

vec4 GetScaleEightValue()
{
    float dx = 1.0/float(Width);
    float dy = 1.0/float(Hight);
	
    vec4 color = texture2D(SrcTexture,textureCoord.xy);
	
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*2.0,0.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*4.0,0.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*6.0,0.0));
    
    color += texture2D(SrcTexture,textureCoord.xy+vec2(0.0,dy*2.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*2.0,dy*2.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*4.0,dy*2.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*6.0,dy*2.0));
    
    color += texture2D(SrcTexture,textureCoord.xy+vec2(0.0,dy*4.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*2.0,dy*4.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*4.0,dy*4.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*6.0,dy*4.0));
    
    color += texture2D(SrcTexture,textureCoord.xy+vec2(0.0,dy*6.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*2.0,dy*6.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*4.0,dy*6.0));
    color += texture2D(SrcTexture,textureCoord.xy+vec2(dx*6.0,dy*6.0));

    color /= 16.0;
    return color ;
}


void main()
{
  DestTexture = GetScaleFourValue() ;
}