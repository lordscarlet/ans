#ifndef BIN_H_458BB345
#define BIN_H_458BB345

#include <stdint.h>

#include "screen.h"
#include "../sauce.h"
#include "../../image/canvas.h"

typedef struct
{
    Screen   *screen;
    uint32_t actual_file_size;
    Sauce    *sauce;
} BinaryFile;

Canvas* load_binary_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: BIN_H_458BB345 */
