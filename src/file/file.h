#ifndef FILE_H_B2772DE4
#define FILE_H_B2772DE4

#include "formats/screen.h"
#include "sauce.h"

typedef enum {
    UNKNOWN, ARTWORX, ASCII, ANSI, ANSIEDIT, BINARY, DIZ, ICE_DRAW, PC_BOARD, TUNDRA, XBIN
} FileType;

typedef struct {
    Screen   *screen;
    uint32_t length;
    Sauce    *sauce;
} TextArtFile;

TextArtFile* read_text_art_file(char *filename);

void debug_text_art_file(TextArtFile *file);

void free_text_art_file(TextArtFile *file);

#endif /* end of include guard: FILE_H_B2772DE4 */
