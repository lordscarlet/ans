#ifndef ARTWORX_H_22DDD857
#define ARTWORX_H_22DDD857

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"

typedef struct
{
    Screen   *screen;
    uint32_t actual_file_size;
    Sauce    *sauce;
} ArtworxFile;

Canvas* load_artworx_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: ARTWORX_H_22DDD857 */
