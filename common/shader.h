#include <kinc/graphics4/shader.h>

#include <string.h>

static void sample_load_shader(char const *filename,
    kinc_g4_shader_t *shader, kinc_g4_shader_type_t shader_type)
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
