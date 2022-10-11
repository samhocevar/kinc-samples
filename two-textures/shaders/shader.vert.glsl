#version 450

in vec2 pos;
in vec2 uv;

out vec2 pass_uv;

void main()
{
    pass_uv = uv;
    gl_Position = vec4(pos.x, pos.y, 0.5, 1.0);
}
