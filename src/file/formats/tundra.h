#ifndef TUNDRA_H_54B22F20
#define TUNDRA_H_54B22F20

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"

typedef struct
{
    uint16_t rows;
    uint8_t *image_bytes;
    uint32_t actual_file_size;
    Sauce *sauce;
} TundraFile;

Canvas* load_tundra_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: TUNDRA_H_54B22F20 */
