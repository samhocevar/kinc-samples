#version 450

in vec2 pos;
in vec2 uv;

out vec2 pass_uv;

uniform vec4 bounds;

void main()
{
    pass_uv = uv;
    vec2 pos2 = mix(bounds.xy, bounds.zw, pos * 0.5 + vec2(0.5));
    gl_Position = vec4(pos2, 0.5, 1.0);
}
