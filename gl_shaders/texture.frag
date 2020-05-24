#version 410

uniform sampler2D image;
uniform float transparency;

in vec3 fPosition;
in vec2 fTexCoord;

out vec4 color;

void main(void)
{
    vec4 col = texture(image, fTexCoord);
    if (col.a < 0.1) discard;
    color = vec4(col.rgb, transparency);
}

