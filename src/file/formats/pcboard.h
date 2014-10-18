#ifndef PCBOARD_H_58B9C971
#define PCBOARD_H_58B9C971

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"
#include "palette.h"
#include "font.h"

typedef struct
{
    Palette  *palette;
    Font     *font;
    uint16_t columns;
    uint16_t rows;
    uint8_t *image_bytes;
    uint32_t actual_file_size;
    Sauce *sauce;
} PCBoardFile;

Canvas* load_pcboard_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: PCBOARD_H_58B9C971 */
