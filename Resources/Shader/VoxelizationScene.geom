#version 450 core
  
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
  
in vec2 GSTextureCoord[];
in vec3 GSNormal[];
in vec3 GSTangent[];
in vec4 GSWorldPostion[] ;
  
out vec2 FSTexcoordCodrd;
out vec3 FSNormal;
out vec3 FSTangent;
out vec4 FSWorldPosition;
out vec4 FSNDCPosition;
  
void main(void)  
{  
   const vec4 E1 = GSWorldPostion[1] - GSWorldPostion[0];
   const vec4 E2 = GSWorldPostion[2] - GSWorldPostion[0];
   vec3 FaceNormal = abs(cross(E1.xyz, E2.xyz));
   for(int i = 0; i < gl_in.length(); ++i)  
   {  
       gl_Position = gl_in[i].gl_Position;
       FSNDCPosition = gl_Position ;
       FSTexcoordCodrd = GSTextureCoord[i];
       FSNormal = GSNormal[i];
       FSTangent = GSTangent[i];
       FSWorldPosition = GSWorldPostion[i];

       // Choice Biggest Rasterization Aera 
       if(FaceNormal.x > FaceNormal.y && FaceNormal.x > FaceNormal.z)
       {
          gl_Position = vec4(gl_Position.y, gl_Position.z, 0, 1.0);
       }
       else if(FaceNormal.y > FaceNormal.x && FaceNormal.y > FaceNormal.z)
       {
          gl_Position = vec4(gl_Position.x, gl_Position.z, 0, 1.0);
       }
       else
       {
          gl_Position = vec4(gl_Position.x, gl_Position.y, 0, 1.0);
       }

       EmitVertex();
   }
   EndPrimitive();  
}  
  