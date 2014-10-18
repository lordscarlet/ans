#ifndef PCBOARD_H_58B9C971
#define PCBOARD_H_58B9C971

#include <stdint.h>

#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"

typedef struct
{
    Screen   *screen;
    uint32_t actual_file_size;
    Sauce    *sauce;
} PCBoardFile;

Canvas* load_pcboard_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: PCBOARD_H_58B9C971 */
