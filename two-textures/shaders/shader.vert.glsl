#version 450

in vec2 pos;
in vec2 uv;

out vec2 ouv;

void main()
{
    ouv = uv;
    gl_Position = vec4(pos.x, pos.y, 0.5, 1.0);
}
