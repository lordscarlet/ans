#ifndef FILE_H_B2772DE4
#define FILE_H_B2772DE4

#include "../image/canvas.h"

typedef enum {
    UNKNOWN, ARTWORX, ANSI, ANSIEDIT, BINARY, ICE_DRAW, PC_BOARD, TUNDRA, XBIN
} FileType;

Canvas* read_file_and_generate_canvas(char const *filename);

#endif /* end of include guard: FILE_H_B2772DE4 */
