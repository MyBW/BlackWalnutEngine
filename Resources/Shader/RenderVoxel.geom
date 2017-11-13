#version 450 core

// 输入的是points类型
layout (points) in;
// 输出的是triangle_strip类型,最多输出顶点数为4
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 ViewMatrix;
uniform mat4 ProjectMatrix;
uniform vec4 VoxelSize ;

in vec3 GSTextureCoord[];

out vec3 FSTextureCoord;
void main()
{
    vec4 VoxelCenterPos = gl_in[0].gl_Position;
    
    vec3 SceneSizeMin = vec3(-1000 , -1000, -1000) ;
    vec3 SceneSizeMax = vec3(1000 , 1000, 1000) ;
    float r = SceneSizeMin.x;
    float l = SceneSizeMax.x;
    float t = SceneSizeMax.y;
    float b = SceneSizeMin.y ;
    float n = SceneSizeMin.z;
    float f = SceneSizeMax.z;
	mat4 OrthoProject = mat4(
      2.0 /(r -l) , 0.0, 0.0, -(r + l)/(r - l),
      0.0, 2/(t -b), 0.0, -(t + b)/(t - b),
      0.0, 0.0, -2/(f - n), - (f + n)/(f -n),
      0.0, 0.0, 0.0, 1.0
    ) ;
   mat4 TestProjectMatrix = transpose(OrthoProject) ;
   mat4 VP = ProjectMatrix * ViewMatrix;
    vec4 VoxelCenterNDC = VP * VoxelCenterPos ;
    	if (VoxelCenterNDC.x < -VoxelCenterNDC.w
		|| VoxelCenterNDC.x > VoxelCenterNDC.w
		|| VoxelCenterNDC.y < -VoxelCenterNDC.w
		|| VoxelCenterNDC.y > VoxelCenterNDC.w
		|| VoxelCenterNDC.z < -VoxelCenterNDC.w
		|| VoxelCenterNDC.z > VoxelCenterNDC.w)
		return;

    FSTextureCoord = GSTextureCoord[0];

// -X
	{
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, .5, .5, 0.0));
		//gs_out.offset = vec3(-.5, .5, .5);
        FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, .5, -.5, 0.0));
		//gs_out.offset = vec3(-.5, .5, -.5);
        FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, -.5, .5, 0.0));
		//gs_out.offset = vec3(-.5, -.5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
		
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, -.5, -.5, 0.0));
		//gs_out.offset = vec3(-.5, -.5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
	}
	EndPrimitive();
	// +X
	{
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, .5, -.5, 0.0));
		//gs_out.offset = vec3(.5, .5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, .5, .5, 0.0));			
		//gs_out.offset = vec3(.5, .5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, -.5, -.5, 0.0));
		//gs_out.offset = vec3(.5, -.5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
		
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, -.5, .5, 0.0));
		//gs_out.offset = vec3(.5, -.5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
	}
	EndPrimitive();
	// -Y
	{
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, -.5, -.5, 0.0));
		//gs_out.offset = vec3(-.5, -.5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, -.5, -.5, 0.0));
		//gs_out.offset = vec3(.5, -.5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, -.5, .5, 0.0));
		//gs_out.offset = vec3(-.5, -.5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
		
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, -.5, .5, 0.0));
		//gs_out.offset = vec3(.5, -.5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
	}
	EndPrimitive();
	// +Y
	{
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, .5, -.5, 0.0));
		//gs_out.offset = vec3(.5, .5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, .5, -.5, 0.0));
		//gs_out.offset = vec3(-.5, .5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, .5, .5, 0.0));
		//gs_out.offset = vec3(.5, .5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
		
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, .5, .5, 0.0));			
		//gs_out.offset = vec3(-.5, .5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
	}
	EndPrimitive();
	// -Z
	{
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, -.5, -.5, 0.0));
		//gs_out.offset = vec3(-.5, -.5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, .5, -.5, 0.0));
		//gs_out.offset = vec3(-.5, .5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, -.5, -.5, 0.0));
		//gs_out.offset = vec3(.5, -.5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
		
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, .5, -.5, 0.0));
		//gs_out.offset = vec3(.5, .5, -.5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
	}
	EndPrimitive();
	// +Z
	{
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, -.5, .5, 0.0));
		//gs_out.offset = vec3(.5, -.5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(.5, .5, .5, 0.0));
		//gs_out.offset = vec3(.5, .5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();

		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, -.5, .5, 0.0));
		//gs_out.offset = vec3(-.5, -.5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
		
		gl_Position = VP * (VoxelCenterPos + VoxelSize * vec4(-.5, .5, .5, 0.0));
		//gs_out.offset = vec3(-.5, .5, .5);
		FSTextureCoord = GSTextureCoord[0];
		EmitVertex();
	}
	EndPrimitive();


}