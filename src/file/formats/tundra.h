#ifndef TUNDRA_H_54B22F20
#define TUNDRA_H_54B22F20

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"

typedef struct
{
    Screen   *screen;
    uint32_t actual_file_size;
    Sauce    *sauce;
} TundraFile;

Canvas* load_tundra_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: TUNDRA_H_54B22F20 */
