#include <stdio.h>
#include <stdlib.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint16_t BINARY_DEFAULT_COLUMNS = 160;

TextArtFile* load_binary_file(char *filename)
{
    TextArtFile *file;
    FILE        *file_ptr;
    file                   = malloc(sizeof(TextArtFile));
    file->screen           = create_screen_with_palette_and_font(CHARACTER_AND_ATTRIBUTE_PAIR, BINARY_PALETTE, CP437_8x16);
    file_ptr               = fopen(filename, "r");
    file->sauce            = get_sauce(file_ptr);
    file->length           = get_real_file_size(file_ptr, file->sauce);
    file->screen->columns  = BINARY_DEFAULT_COLUMNS;
    file->screen->rows     = file->length / 2 / BINARY_DEFAULT_COLUMNS;
    load_screen(file->screen, file_ptr);
    fclose(file_ptr);
    use_sauce_ansiflags(file->screen, file->sauce);
    return file;
}
