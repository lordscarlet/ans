#ifndef FILE_H_B2772DE4
#define FILE_H_B2772DE4

#include "formats/screen.h"
#include "sauce.h"
#include "../image/canvas.h"

typedef enum {
    UNKNOWN, ARTWORX, ANSI, ANSIEDIT, BINARY, ICE_DRAW, PC_BOARD, TUNDRA, XBIN
} FileType;

typedef struct {
    Screen   *screen;
    uint32_t length;
    Sauce    *sauce;
} TextArtFile;

Canvas* read_file_and_generate_canvas(char const *filename);

void debug_text_art_file(TextArtFile *file);

void free_text_art_file(TextArtFile *file);

#endif /* end of include guard: FILE_H_B2772DE4 */
