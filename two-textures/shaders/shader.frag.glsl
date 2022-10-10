#version 450

out vec4 FragColor;
in vec2 uv;

uniform sampler2D sprite;
uniform sampler2D palette;

void main()
{
    float shade = texture(sprite, uv).x;
    FragColor = texture(palette, vec2(shade, 0.5));
}
