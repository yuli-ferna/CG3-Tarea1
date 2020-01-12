  
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aText;

//Uniforms MVP matrix
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Proj;

out vec2 coordText;

void main()
{
    gl_Position = /*Proj * View * Model */ vec4(aPos, 1.0);
    coordText = aText;
}