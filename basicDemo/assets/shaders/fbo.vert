  
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aText;

//Uniforms MVP matrix

out vec2 coordText;

void main()
{
    gl_Position = /*Proj * View * Model */ vec4(aPos, 1.0);
    coordText = aText;
}