#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/rendertarget.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/vertexbuffer.h>

#include <kinc/io/filereader.h>
#include <kinc/input/gamepad.h>
#include <kinc/input/keyboard.h>
#include <kinc/log.h>
#include <kinc/system.h>

#include <math.h>

#include "../common/shader.h"

static kinc_g4_pipeline_t pipeline;
static kinc_g4_shader_t vertex_shader;
static kinc_g4_shader_t fragment_shader;
static kinc_g4_constant_location_t axes_loc;
static kinc_g4_constant_location_t buttons_loc;

static kinc_g4_vertex_structure_t structure;
static kinc_g4_index_buffer_t indices;
static kinc_g4_vertex_buffer_t vertices;

typedef struct state
{
    float axes[4];
    int32_t buttons;
}
state_t;

static void axis_callback(int gamepad, int axis, float value, void *userdata)
{
    state_t *s = (state_t *)userdata;
    if (axis >= 0 && axis < 4)
        s->axes[axis] = value;
}

static void button_callback(int gamepad, int button, float value, void *userdata)
{
    state_t *s = (state_t *)userdata;
    int bit = 1 << button;
    s->buttons = (s->buttons & ~bit) | (value ? bit : 0);
}

static void key_down_callback(int code, void *userdata)
{
    state_t *s = (state_t *)userdata;
    int bit = code >= KINC_KEY_A && code <= KINC_KEY_Z ? 1 << (code - KINC_KEY_A) : 0;
    s->buttons |= bit;
}

static void key_up_callback(int code, void *userdata)
{
    state_t *s = (state_t *)userdata;
    int bit = code >= KINC_KEY_A && code <= KINC_KEY_Z ? 1 << (code - KINC_KEY_A) : 0;
    s->buttons &= ~bit;
}

static void update(void *userdata)
{
    state_t *s = (state_t *)userdata;

    kinc_g4_begin(0);
    kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFFFF8000, 0.0f, 0);

    kinc_g4_set_pipeline(&pipeline);

    // Upload gamepad state to shader
    kinc_g4_set_floats(axes_loc, s->axes, 4);
    kinc_g4_set_int(buttons_loc, s->buttons);

    // Render a fullscreen quad
    kinc_g4_set_vertex_buffer(&vertices);
    kinc_g4_set_index_buffer(&indices);
    kinc_g4_draw_indexed_vertices();

    kinc_g4_end(0);
    kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv)
{
    state_t state;

    kinc_init("Gamepad", 1280, 720, NULL, NULL);
    kinc_set_update_callback(update, &state);

    kinc_g4_vertex_structure_init(&structure);
    kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_F32_2X);
    kinc_g4_vertex_structure_add(&structure, "uv", KINC_G4_VERTEX_DATA_F32_2X);

    sample_load_shader("gamepad.vert", &vertex_shader, KINC_G4_SHADER_TYPE_VERTEX);
    sample_load_shader("gamepad.frag", &fragment_shader, KINC_G4_SHADER_TYPE_FRAGMENT);

    kinc_g4_pipeline_init(&pipeline);
    pipeline.vertex_shader = &vertex_shader;
    pipeline.fragment_shader = &fragment_shader;
    pipeline.input_layout[0] = &structure;
    pipeline.input_layout[1] = NULL;
    pipeline.blend_operation = KINC_G4_BLENDOP_ADD;
    pipeline.blend_source = KINC_G4_BLEND_SOURCE_ALPHA;
    pipeline.blend_destination = KINC_G4_BLEND_INV_SOURCE_ALPHA;
    kinc_g4_pipeline_compile(&pipeline);

    axes_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "axes");
    buttons_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "buttons");

    // Vertex buffer
    kinc_g4_vertex_buffer_init(&vertices, 4, &structure, KINC_G4_USAGE_DYNAMIC, 0);
    float vert_data[] = { -1, -1, 0, 0, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1 };
    float *vert = kinc_g4_vertex_buffer_lock_all(&vertices);
    memcpy(vert, vert_data, sizeof(vert_data));
    kinc_g4_vertex_buffer_unlock_all(&vertices);

    // Index buffer
    uint16_t index_data[] = { 0, 1, 2, 2, 3, 0 };
    uint16_t index_len = sizeof(index_data) / sizeof(*index_data);
    kinc_g4_index_buffer_init(&indices, index_len, KINC_G4_INDEX_BUFFER_FORMAT_16BIT, KINC_G4_USAGE_STATIC);
    uint16_t *index = (uint16_t *)kinc_g4_index_buffer_lock(&indices, 0, index_len);
    memcpy(index, index_data, sizeof(index_data));
    kinc_g4_index_buffer_unlock(&indices, index_len);

    kinc_gamepad_set_axis_callback(axis_callback, &state);
    kinc_gamepad_set_button_callback(button_callback, &state);
    kinc_keyboard_set_key_down_callback(key_down_callback, &state);
    kinc_keyboard_set_key_up_callback(key_up_callback, &state);

    kinc_start();

    return 0;
}
