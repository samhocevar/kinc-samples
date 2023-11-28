#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/rendertarget.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/vertexbuffer.h>

#include <kinc/io/filereader.h>
#include <kinc/math/random.h>
#include <kinc/system.h>

#include <math.h>

#include "../common/shader.h"

static kinc_g4_pipeline_t pipeline;
static kinc_g4_shader_t vertex_shader;
static kinc_g4_shader_t fragment_shader;
static kinc_g4_constant_location_t bounds_loc;
static kinc_g4_texture_unit_t image_unit;

static kinc_g4_vertex_structure_t structure;
static kinc_g4_index_buffer_t indices;
static kinc_g4_vertex_buffer_t vertices;

static kinc_g4_texture_t image;

static void bind_texture(void)
{
    // Bind the image texture
    kinc_g4_set_texture(image_unit, &image);
    kinc_g4_set_texture_mipmap_filter(image_unit, KINC_G4_MIPMAP_FILTER_NONE);
    kinc_g4_set_texture_addressing(image_unit, KINC_G4_TEXTURE_DIRECTION_U, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
    kinc_g4_set_texture_addressing(image_unit, KINC_G4_TEXTURE_DIRECTION_V, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
    kinc_g4_set_texture_minification_filter(image_unit, KINC_G4_TEXTURE_FILTER_LINEAR);
    kinc_g4_set_texture_magnification_filter(image_unit, KINC_G4_TEXTURE_FILTER_LINEAR);
}

static void update(void *data)
{
    kinc_g4_begin(0);
    kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF008080, 0.0f, 0);

    kinc_g4_set_pipeline(&pipeline);

    // On even frames, only bind the texture once
    if ((int)kinc_time() % 2 == 0)
        bind_texture();

    for (int n = 0; n < 4; ++n)
    {
        // On odd frames, bind the texture once per quad
        if ((int)kinc_time() % 2 == 1)
            bind_texture();

        kinc_g4_set_vertex_buffer(&vertices);
        kinc_g4_set_index_buffer(&indices);

        // Render quad
        float bounds[] = { (n % 2 - 1), (n / 2 - 1), (n % 2), (n / 2) };
        kinc_g4_set_floats(bounds_loc, bounds, 4);
        kinc_g4_draw_indexed_vertices();
    }

    kinc_g4_end(0);
    kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv)
{
    kinc_init("Starfield", 1280, 720, NULL, NULL);
    kinc_random_init(0);
    kinc_set_update_callback(update, NULL);

    kinc_g4_vertex_structure_init(&structure);
    kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_F32_2X);
    kinc_g4_vertex_structure_add(&structure, "uv", KINC_G4_VERTEX_DATA_F32_2X);

    sample_load_shader("starfield.vert", &vertex_shader, KINC_G4_SHADER_TYPE_VERTEX);
    sample_load_shader("starfield.frag", &fragment_shader, KINC_G4_SHADER_TYPE_FRAGMENT);

    kinc_g4_pipeline_init(&pipeline);
    pipeline.vertex_shader = &vertex_shader;
    pipeline.fragment_shader = &fragment_shader;
    pipeline.input_layout[0] = &structure;
    pipeline.input_layout[1] = NULL;
    kinc_g4_pipeline_compile(&pipeline);

    bounds_loc = kinc_g4_pipeline_get_constant_location(&pipeline, "bounds");
    image_unit = kinc_g4_pipeline_get_texture_unit(&pipeline, "image");

    // Vertex buffer
    kinc_g4_vertex_buffer_init(&vertices, 4, &structure, KINC_G4_USAGE_DYNAMIC, 0);
    float vert_data[] = { -1, -1, 0, 0, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1 };
    float *vert = kinc_g4_vertex_buffer_lock_all(&vertices);
    memcpy(vert, vert_data, sizeof(vert_data));
    kinc_g4_vertex_buffer_unlock_all(&vertices);

    // Index buffer
    kinc_g4_index_buffer_init(&indices, 6, KINC_G4_INDEX_BUFFER_FORMAT_16BIT, KINC_G4_USAGE_STATIC);
    uint16_t index_data[] = { 0, 1, 2, 2, 3, 0 };
    uint16_t index_len = sizeof(index_data) / sizeof(*index_data);
    uint16_t *index = (uint16_t *)kinc_g4_index_buffer_lock(&indices, 0, index_len);
    memcpy(index, index_data, sizeof(index_data));
    kinc_g4_index_buffer_unlock(&indices, index_len);

    // Generate starfield image
    kinc_g4_texture_init(&image, 320, 180, KINC_IMAGE_FORMAT_GREY8);
    uint8_t* pixels = kinc_g4_texture_lock(&image);
    int stride = kinc_g4_texture_stride(&image);
    for (int n = 0; n < 500; ++n)
    {
        int x = kinc_random_get_in(0, 320), y = kinc_random_get_in(0, 180);
        pixels[y * stride + x] = kinc_random_get_in(128, 255);
    }
    kinc_g4_texture_unlock(&image);
 
    kinc_start();

    kinc_g4_texture_destroy(&image);

    return 0;
}
