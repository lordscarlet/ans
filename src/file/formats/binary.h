#ifndef BIN_H_458BB345
#define BIN_H_458BB345

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"
#include "palette.h"

typedef struct
{
    Palette  *palette;
    uint16_t rows;
    uint8_t  *image_bytes;
    uint32_t actual_file_size;
    Sauce *sauce;
} BinaryFile;

Canvas* load_binary_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: BIN_H_458BB345 */
