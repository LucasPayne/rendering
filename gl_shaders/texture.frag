#version 410

uniform sampler2D image;

in vec3 fPosition;
in vec2 fTexCoord;

out vec4 color;

void main(void)
{
    color = vec4(texture(image, fTexCoord).rgb, 1);
}

