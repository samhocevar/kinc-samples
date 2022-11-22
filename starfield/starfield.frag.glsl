#version 450

out vec4 FragColor;
in vec2 pass_uv;

uniform sampler2D image;

void main()
{
    vec4 color = texture(image, pass_uv).xxxx;
    color.r = pow(color.r, 2);
    color.g = pow(color.g, 2);
    FragColor = color;
}
