#version 430  
  
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
  
in  vec2 textureCoord[];  
  
out vec2 GF_TexcoordCodrd;  
  
void main(void)  
{  
   for(int i = 0; i < gl_in.length(); ++i)  
   {  
       gl_Position = gl_in[i].gl_Position;
       gl_Position.xy = gl_Position.xy * 0.5 ;
       GF_TexcoordCodrd = textureCoord[i];
       EmitVertex();
   }
   EndPrimitive();  
}  
  