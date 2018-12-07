#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 myColor;
out vec2 TexCoord;

uniform mat4 perspective;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = perspective * view * model * vec4(aPos, 1.0f);
    myColor = aColor;
}
