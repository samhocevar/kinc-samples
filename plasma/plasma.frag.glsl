#version 450

out vec4 FragColor;
in vec2 pass_uv;

uniform sampler2D image;
uniform sampler2D palette;

void main()
{
    float shade = texture(image, pass_uv).x;
    FragColor = texture(palette, vec2(shade, 0.5));
}
