#version 330 core
layout (location = 0) in vec3 aPos;
uniform vec4 aColor;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;
//
out vec4 ourColor; 

void main()
{
	mat4 mvpx = projection * view * model;
	mat4 mvpy = model * view * projection;
    gl_Position =  vec4(aPos, 1.0) * mvpx;
	//gl_Position =  mvpy * vec4(aPos, 1.0);
	ourColor = aColor;
}
