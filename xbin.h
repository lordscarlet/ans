#ifndef XBIN_H_1304F866
#define XBIN_H_1304F866

#include <stdint.h>
#include <stdbool.h>

#include "canvas.h"

typedef struct
{
    uint16_t columns;
    uint16_t rows;
    uint8_t font_height;
    bool flag_char_512;
    bool flag_non_blink;
    uint8_t *palette_bytes;
    uint8_t *font_bytes;
    uint32_t font_bytes_length;
    uint8_t *image_bytes;
    uint32_t image_bytes_length;
} XBin_File;

XBin_File* load_xbin(char const *filename);

void free_xbin_file(XBin_File *file);

void debug_xbin_file(XBin_File *file);

Canvas* xbin_file_to_canvas(XBin_File *file);

#endif /* end of include guard: XBIN_H_1304F866 */
