#version 330 core
layout (location = 0) in vec3 aPos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 mvp;


void main()
{
    gl_Position =   projection * view * model * vec4(aPos, 1.0);
    TexCoord = vec2(aPos.x/2.0+0.5, 0.5-aPos.y/2.0);
};
