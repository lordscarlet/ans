#ifndef XBIN_H_1304F866
#define XBIN_H_1304F866

#include <stdint.h>
#include <stdbool.h>

#include "../../image/canvas.h"
#include "../sauce.h"
#include "font.h"

typedef struct
{
    uint16_t columns;
    uint16_t rows;
    uint8_t  font_height;
    bool     flag_char_512;
    bool     flag_non_blink;
    uint8_t  palette_bytes[48];
    Font     *font;
    uint8_t  *image_bytes;
    uint32_t image_bytes_length;
    uint32_t actual_file_size;
    Sauce    *sauce;
} XBinFile;

Canvas* load_xbin_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: XBIN_H_1304F866 */
