#ifndef ICEDRAW_H_D90DF873
#define ICEDRAW_H_D90DF873

#include <stdint.h>

#include "sauce.h"
#include "../image/canvas.h"

typedef struct
{
    uint16_t rows;
    uint8_t  palette_bytes[48];
    uint8_t  font_bytes[4096];
    uint32_t actual_file_size;
    uint8_t *image_bytes;
    Sauce *sauce;
} IceDrawFile;

Canvas* load_ice_draw_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: ICEDRAW_H_D90DF873 */
