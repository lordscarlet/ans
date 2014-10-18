#ifndef XBIN_H_1304F866
#define XBIN_H_1304F866

#include <stdint.h>
#include <stdbool.h>

#include "../../image/canvas.h"
#include "../sauce.h"
#include "palette.h"
#include "font.h"

typedef struct
{
    uint16_t columns;
    uint16_t rows;
    bool     flag_non_blink;
    Palette  *palette;
    Font     *font;
    uint8_t  *image_bytes;
    uint32_t actual_file_size;
    Sauce    *sauce;
} XBinFile;

Canvas* load_xbin_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: XBIN_H_1304F866 */
