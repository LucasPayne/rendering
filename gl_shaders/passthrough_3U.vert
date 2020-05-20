#version 410

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec2 vTexCoord;

out vec3 fPosition;
out vec2 fTexCoord;

void main(void)
{
    gl_Position = vec4(vPosition, 1);
    fPosition = vPosition;
    fTexCoord = vTexCoord;
}

