#include <kinc/graphics4/graphics.h>
#include <kinc/graphics4/indexbuffer.h>
#include <kinc/graphics4/pipeline.h>
#include <kinc/graphics4/rendertarget.h>
#include <kinc/graphics4/shader.h>
#include <kinc/graphics4/texture.h>
#include <kinc/graphics4/vertexbuffer.h>

#include <kinc/io/filereader.h>
#include <kinc/system.h>

#include <string.h>
#include <math.h>

static kinc_g4_pipeline_t pipeline;
static kinc_g4_shader_t vertex_shader;
static kinc_g4_shader_t fragment_shader;
static kinc_g4_texture_unit_t image_unit;
static kinc_g4_texture_unit_t palette_unit;

static kinc_g4_vertex_structure_t structure;
static kinc_g4_index_buffer_t indices;
static kinc_g4_vertex_buffer_t vertices;

static kinc_g4_texture_t image;
static kinc_g4_texture_t palette;

static void load_shader(const char *filename, kinc_g4_shader_t *shader, kinc_g4_shader_type_t shader_type)
{
    kinc_file_reader_t file;
    kinc_file_reader_open(&file, filename, KINC_FILE_TYPE_ASSET);
    size_t data_size = kinc_file_reader_size(&file);
    uint8_t *data = malloc(data_size);
    kinc_file_reader_read(&file, data, data_size);
    kinc_file_reader_close(&file);
    kinc_g4_shader_init(shader, data, data_size, shader_type);
    free(data);
}

static void update(void)
{
    kinc_g4_begin(0);
    kinc_g4_clear(KINC_G4_CLEAR_COLOR, 0xFF008080, 0.0f, 0);

    // Upload a cyclic palette texture
    uint8_t *pixels = kinc_g4_texture_lock(&palette);
    for (int x = 0; x < palette.tex_width; ++x)
    {
        pixels[4 * x + 0] = (uint8_t)(127.5 * (1 + sin(x * 6.2832 / 256 + kinc_time() * 1.3)));
        pixels[4 * x + 1] = (uint8_t)(127.5 * (1 + sin(x * 6.2832 / 256 + kinc_time() * 2.4)));
        pixels[4 * x + 2] = (uint8_t)(127.5 * (1 + sin(x * 6.2832 / 128 + kinc_time() * 4.1)));
        pixels[4 * x + 3] = 255;
    }
    kinc_g4_texture_unlock(&palette);
        
    kinc_g4_set_pipeline(&pipeline);

    // Bind the image texture
    kinc_g4_set_texture(image_unit, &image);
    kinc_g4_set_texture_mipmap_filter(image_unit, KINC_G4_MIPMAP_FILTER_NONE);
    kinc_g4_set_texture_addressing(image_unit, KINC_G4_TEXTURE_DIRECTION_U, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
    kinc_g4_set_texture_addressing(image_unit, KINC_G4_TEXTURE_DIRECTION_V, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
    kinc_g4_set_texture_minification_filter(image_unit, KINC_G4_TEXTURE_FILTER_POINT);
    kinc_g4_set_texture_magnification_filter(image_unit, KINC_G4_TEXTURE_FILTER_POINT);

    // Bind the palette texture
    kinc_g4_set_texture(palette_unit, &palette);
    kinc_g4_set_texture_mipmap_filter(palette_unit, KINC_G4_MIPMAP_FILTER_NONE);
    kinc_g4_set_texture_addressing(palette_unit, KINC_G4_TEXTURE_DIRECTION_U, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
    kinc_g4_set_texture_addressing(palette_unit, KINC_G4_TEXTURE_DIRECTION_V, KINC_G4_TEXTURE_ADDRESSING_CLAMP);
    kinc_g4_set_texture_minification_filter(palette_unit, KINC_G4_TEXTURE_FILTER_LINEAR);
    kinc_g4_set_texture_magnification_filter(palette_unit, KINC_G4_TEXTURE_FILTER_LINEAR);

    // Render a fullscreen quad
    kinc_g4_set_vertex_buffer(&vertices);
    kinc_g4_set_index_buffer(&indices);
    kinc_g4_draw_indexed_vertices();

    kinc_g4_end(0);
    kinc_g4_swap_buffers();
}

int kickstart(int argc, char **argv)
{
    kinc_init("Two Textures", 1280, 720, NULL, NULL);
    kinc_set_update_callback(update);

    kinc_g4_vertex_structure_init(&structure);
    kinc_g4_vertex_structure_add(&structure, "pos", KINC_G4_VERTEX_DATA_F32_2X);
    kinc_g4_vertex_structure_add(&structure, "uv", KINC_G4_VERTEX_DATA_F32_2X);

    load_shader("shader.vert", &vertex_shader, KINC_G4_SHADER_TYPE_VERTEX);
    load_shader("shader.frag", &fragment_shader, KINC_G4_SHADER_TYPE_FRAGMENT);

    kinc_g4_pipeline_init(&pipeline);
    pipeline.vertex_shader = &vertex_shader;
    pipeline.fragment_shader = &fragment_shader;
    pipeline.input_layout[0] = &structure;
    pipeline.input_layout[1] = NULL;
    kinc_g4_pipeline_compile(&pipeline);

    image_unit = kinc_g4_pipeline_get_texture_unit(&pipeline, "image");
    palette_unit = kinc_g4_pipeline_get_texture_unit(&pipeline, "palette");

    // Vertex buffer
    kinc_g4_vertex_buffer_init(&vertices, 4, &structure, KINC_G4_USAGE_DYNAMIC, 0);
    float vert_data[] = { -1, -1, 0, 0, 1, -1, 1, 0, 1, 1, 1, 1, -1, 1, 0, 1 };
    float *vert = kinc_g4_vertex_buffer_lock_all(&vertices);
    memcpy(vert, vert_data, sizeof(vert_data));
    kinc_g4_vertex_buffer_unlock_all(&vertices);

    // Index buffer
    kinc_g4_index_buffer_init(&indices, 6, KINC_G4_INDEX_BUFFER_FORMAT_16BIT, KINC_G4_USAGE_STATIC);
    uint16_t index_data[] = { 0, 1, 2, 2, 3, 0 };
    uint16_t *index = (uint16_t *)kinc_g4_index_buffer_lock(&indices);
    memcpy(index, index_data, sizeof(index_data));
    kinc_g4_index_buffer_unlock(&indices);

    // Generate grayscale image
    kinc_g4_texture_init(&image, 320, 180, KINC_IMAGE_FORMAT_GREY8);
    uint8_t* pixels = kinc_g4_texture_lock(&image);
    int stride = kinc_g4_texture_stride(&image);
    for (int y = 0; y < image.tex_height; ++y)
    {
        for (int x = 0; x < image.tex_width; ++x)
        {
            double dx = (double)x / image.tex_width;
            double dy = (double)y / image.tex_height;
            int val = (int)(127 + 35 * sin(3 + dx * 13) + 37 * sin(dx * 14 + 11 / (dy + 0.5)) + 56 * sin(15 * dy + 10 * dx * dx));
            pixels[y * stride + x] = val + y - x + ((x + x - (y & 1)) & 3) * 15;
        }
    }
    kinc_g4_texture_unlock(&image);
 
    // Palette texture
    kinc_g4_texture_init(&palette, 256, 1, KINC_IMAGE_FORMAT_RGBA32);

    kinc_start();

    kinc_g4_texture_destroy(&image);
    kinc_g4_texture_destroy(&palette);

    return 0;
}
