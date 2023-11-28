#include <kinc/error.h>
#include <kinc/graphics4/shader.h>

#include <string.h>

static void sample_load_shader(char const *filename,
    kinc_g4_shader_t *shader, kinc_g4_shader_type_t shader_type)
{
    kinc_file_reader_t file;
    if (!kinc_file_reader_open(&file, filename, KINC_FILE_TYPE_ASSET))
    {
        kinc_error_message("failed to open shader: %s", filename);
        return;
    }
    size_t data_size = kinc_file_reader_size(&file);
    uint8_t *data = malloc(data_size);
    kinc_file_reader_read(&file, data, data_size);
    kinc_file_reader_close(&file);
    kinc_g4_shader_init(shader, data, data_size, shader_type);
    free(data);
}
