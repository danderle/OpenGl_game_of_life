#version 330 core

out vec4 FragColor;

uniform vec4 color;
in vec3 myColor;
void main()
{
    FragColor = color;
}

