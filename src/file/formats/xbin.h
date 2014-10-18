#ifndef XBIN_H_1304F866
#define XBIN_H_1304F866

#include <stdint.h>
#include <stdbool.h>

#include "screen.h"
#include "../sauce.h"
#include "../../image/canvas.h"

typedef struct
{
    Screen   *screen;
    uint32_t actual_file_size;
    Sauce    *sauce;
} XBinFile;

Canvas* load_xbin_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: XBIN_H_1304F866 */
