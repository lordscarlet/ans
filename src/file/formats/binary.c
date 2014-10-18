#include <stdio.h>
#include <stdlib.h>

#include "binary.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"
#include "palette.h"
#include "font.h"

uint16_t BINARY_DEFAULT_COLUMNS = 160;

BinaryFile* load_binary(char const *filename)
{
    BinaryFile *file;
    FILE       *file_ptr;
    file                   = malloc(sizeof(BinaryFile));
    file->screen           = create_screen_with_palette_and_font(CHARACTER_AND_ATTRIBUTE_PAIR, BINARY_PALETTE, CP437_8x16);
    file_ptr               = fopen(filename, "r");
    file->sauce            = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    file->screen->columns  = BINARY_DEFAULT_COLUMNS;
    file->screen->rows     = file->actual_file_size / 2 / BINARY_DEFAULT_COLUMNS;
    load_screen(file->screen, file_ptr);
    fclose(file_ptr);
    use_sauce_ansiflags(file->screen, file->sauce);
    return file;
}

void free_binary_file(BinaryFile *file)
{
    if(file != NULL)
    {
        free_screen(file->screen);
        if(file->sauce != NULL)
        {
            free(file->sauce);
        }
        free(file);
    }
}

void debug_binary_file(BinaryFile *file)
{
    debug_screen(file->screen);
    printf("Binary actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* load_binary_file_and_generate_canvas(char const *filename)
{
    BinaryFile* file = load_binary(filename);
    debug_binary_file(file);
    Canvas *canvas = screen_to_canvas(file->screen);
    free_binary_file(file);
    return canvas;
}