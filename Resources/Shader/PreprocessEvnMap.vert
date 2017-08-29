#version 430 core

layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
uniform int Index ;
in vec2 Texture_Coordinates ;
uniform mat4 ViewMatrix;

out vec3 WorldPosition ;
void main()
{
	WorldPosition = Position ;
    const float Width = 50; //输入的立方体模型边长是50
    float r = Width;
    float l = -Width;
    float t = Width ;
    float b = -Width ;
    float n =  - 1;
    float f1 = -Width - 1;
     mat4 OrthoProject = mat4(
      2.0 /(r -l) , 0.0, 0.0, -(r + l)/(r - l),
      0.0, 2/(t -b), 0.0, -(t + b)/(t - b),
      0.0, 0.0, 1/(f1 - n), - n/(f1 -n),
      0.0, 0.0, 0.0, 1.0
    ) ;
    mat4 ProjectMatrix = transpose(OrthoProject) ;


        float tanHalfFovy = tan(3.1415926 / 4.0);
        float aspect = 1.0 ;
        float zFar = 90 ;
        float zNear = 30 ;
		mat4 Result ;
		Result[0][0] = 1.0/ (aspect * tanHalfFovy);
		Result[1][1] = 1.0 / (tanHalfFovy);
		Result[2][2] = - (zFar + zNear) / (zFar - zNear);
		Result[2][3] = - 1;
		Result[3][2] = - (2 * zFar * zNear) / (zFar - zNear);
          
        vec3 eye = vec3(0.0) ;
        vec3 center = vec3(0.0) ;
        vec3 up= vec3(0.0) ;
        if(Index == 0) 
        {
          center = vec3( 1.0 ,0.0, 0.0) ;
          up = vec3(0.0 , 1.0 , 0.0) ;
        }
		if(Index == 1) 
		{
          center = vec3( -1.0 ,0.0, 0.0) ;
          up = vec3(0.0 , 1.0 , 0.0) ;
        }
		if(Index == 2)
		{
          center = vec3( 0.0 ,1.0, 0.0) ;
          up = vec3(0.0 , 0.0 , 1.0) ;
        }
		if(Index == 3) 
		{
          center = vec3( 0.0 ,-1.0, 0.0) ;
          up = vec3(0.0 , 0.0 , 1.0) ;
        }
		if(Index == 4)
		{
          center = vec3( 0.0 ,0.0, 1.0) ;
          up = vec3(0.0 , 1.0 , 0.0) ;
        }
		if(Index == 5) 
		{
          center = vec3( 0.0 ,0.0, -1.0) ;
          up = vec3(0.0 , 1.0 , 0.0) ;
        }

        vec3 f =  normalize(center - eye);
		vec3 s = normalize(cross(up, f));
		vec3 u = cross(f, s);

		mat4 VM ;
		VM[0][0] = s.x;
		VM[1][0] = s.y;
		VM[2][0] = s.z;
		VM[0][1] = u.x;
		VM[1][1] = u.y;
		VM[2][1] = u.z;
		VM[0][2] = f.x;
		VM[1][2] = f.y;
		VM[2][2] = f.z;
		VM[3][0] = -dot(s, eye);
		VM[3][1] = -dot(u, eye);
		VM[3][2] = -dot(f, eye);
		VM[3][3] = 1.0 ;


    gl_Position = ProjectMatrix * ViewMatrix * vec4(Position, 1.0) ;
}