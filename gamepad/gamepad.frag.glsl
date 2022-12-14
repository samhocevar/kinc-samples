#version 450

#define MAX_BUTTONS 16

out vec4 FragColor;
in vec2 pass_uv;

uniform vec4 axes;
uniform int buttons;

vec2 button_pos[MAX_BUTTONS] =
{
    {  0.40, -0.05 }, // ABXY
    {  0.50,  0.05 },
    {  0.30,  0.05 },
    {  0.40,  0.15 },
    { -0.75,  0.15 }, // Triggers
    {  0.75,  0.15 },
    { -0.75,  0.24 },
    {  0.75,  0.24 },
    { -0.10,  0.15 }, // Start/pause
    {  0.10,  0.15 },
    { -0.20, -0.18 }, // Thumbs
    {  0.20, -0.18 },
    { -0.40,  0.15 }, // D-pad
    { -0.40, -0.05 },
    { -0.50,  0.05 },
    { -0.30,  0.05 },
};

int round_buttons = 0x0c0f;
int flat_buttons = 0x0330;

//
// Transforms
//

vec2 translate(vec2 p, vec2 v)
{
    return p - v;
}

//
// Primitives
//

float circle(vec2 p, float r)
{
    return length(p) - r;
}

float box(vec2 p, vec2 ext)
{
    vec2 d = abs(p) - ext;
    // inside distance + outside distance
    return length(max(d, 0)) + min(max(d.x, d.y), 0);
}

float segment(vec2 p, vec2 a, vec2 b, float w)
{
    vec2 d = b - a;
    float t = dot(p - a, d) / dot(d, d);
    return distance(p, a + clamp(t, 0.0, 1.0) * d) - w;
}

//
// Operators
//

float outline(float t, float w)
{
    return abs(t) - w;
}

float merge(float t1, float t2, float r)
{
    vec2 v = min(vec2(t1 - r, t2 - r), 0);
    // inside distance + outside distance
    return max(min(t1, t2), r) - length(v);
}

const float RATIO = 1280.0 / 720.0;

const vec4 white = vec4(1.0);
const vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
const vec4 red =   vec4(1.0, 0.0, 0.0, 1.0);
const vec4 green = vec4(0.0, 1.0, 0.0, 1.0);

vec4 blend(vec4 src, vec4 dst, float sdf)
{
    float alpha = smoothstep(1.5 / 720.0, 0., sdf);
    return mix(src, dst, alpha);
}

void main()
{
    vec2 p = (pass_uv - vec2(0.5)) * vec2(RATIO, 1.0);

    float width = 0.004;
    float sdf;

    vec4 color = vec4(0.);

    // Draw gamepad
    sdf = box(translate(p, vec2(0.0, button_pos[1].y)), vec2(0.40, 0.20));
    sdf = merge(circle(translate(p, (button_pos[0] + button_pos[3]) * 0.5), 0.24), sdf, 0.03);
    sdf = merge(circle(translate(p, (button_pos[12] + button_pos[13]) * 0.5), 0.24), sdf, 0.03);
    sdf = merge(circle(translate(p, button_pos[10]), 0.14), sdf, 0.03);
    sdf = merge(circle(translate(p, button_pos[11]), 0.14), sdf, 0.03);
    sdf = min(box(translate(p, mix(button_pos[4], button_pos[6], 2./3)), vec2(0.07, 0.105)), sdf);
    sdf = min(box(translate(p, mix(button_pos[5], button_pos[7], 2./3)), vec2(0.07, 0.105)), sdf);
    color = blend(color, white, sdf);
    color = blend(color, black, outline(sdf, width));

    // Draw buttons
    for (int i = 0; i < MAX_BUTTONS; ++i)
    {
        vec2 pos = button_pos[i];
        if (i == 10 || i == 11)
        {
            // Draw axes for thumbs, and shift button position
            sdf = outline(circle(translate(p, pos), 0.08), width);
            color = blend(color, black, sdf);
            vec2 d = 0.1 * vec2(axes[(i - 10) * 2], axes[(i - 10) * 2 + 1]);
            sdf = segment(p, pos, pos + d, width);
            color = blend(color, black, sdf);
            pos += d;
        }

        if ((round_buttons & (1 << i)) != 0)
            sdf = circle(translate(p, pos), 0.05);
        else if ((flat_buttons & (1 << i)) != 0)
            sdf = box(translate(p, pos), vec2(0.04, 0.02));
        else
            sdf = box(translate(p, pos), vec2(0.04));
        color = blend(color, (buttons & (1 << i)) != 0 ? green : red, sdf);
        color = blend(color, black, outline(sdf, width));
    }

    FragColor = color;
}
