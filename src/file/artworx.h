#ifndef ARTWORX_H_22DDD857
#define ARTWORX_H_22DDD857

#include <stdint.h>

#include "sauce.h"
#include "../image/canvas.h"

typedef struct
{
    uint8_t palette_bytes[48];
    uint8_t font_bytes[4096];
    uint16_t rows;
    uint8_t *image_bytes;
    uint32_t image_bytes_length;
    uint32_t actual_file_size;
    Sauce *sauce;
} ArtworxFile;

Canvas* load_artworx_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: ARTWORX_H_22DDD857 */
