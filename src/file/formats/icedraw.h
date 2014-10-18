#ifndef ICEDRAW_H_D90DF873
#define ICEDRAW_H_D90DF873

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"

typedef struct
{
    Screen   *screen;
    uint32_t actual_file_size;
    Sauce    *sauce;
} IceDrawFile;

Canvas* load_ice_draw_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: ICEDRAW_H_D90DF873 */
