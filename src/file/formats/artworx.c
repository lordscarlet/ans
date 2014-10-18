#include <stdio.h>
#include <stdlib.h>

#include "artworx.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"
#include "palette.h"
#include "font.h"

uint8_t  ARTWORX_PALETTE_ORDERING[] = {0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63};
uint32_t ARTWORX_VERSION_LENGTH     = 1;
uint32_t ARTWORX_PALETTE_LENGTH     = 192;
uint32_t ARTWORX_FONT_LENGTH        = 4096;
uint16_t ARTWORX_DEFAULT_COLUMNS    = 80;

ArtworxFile* load_artworx(char const *filename)
{
    uint8_t     version_number;
    uint32_t    image_bytes_length;
    FILE        *file_ptr;
    ArtworxFile *file;
    uint8_t     full_palette[ARTWORX_PALETTE_LENGTH];
    file                   = malloc(sizeof(ArtworxFile));
    file->screen           = create_screen(CHARACTER_AND_ATTRIBUTE_PAIR);
    file->screen->palette  = create_new_palette();
    file_ptr               = fopen(filename, "r");
    file->sauce            = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    fread(&version_number, 1, ARTWORX_VERSION_LENGTH, file_ptr);
    fread(full_palette, 1, ARTWORX_PALETTE_LENGTH, file_ptr);
    for(uint8_t i = 0; i < 16; i += 1)
    {
        for(uint8_t j = 0, from = ARTWORX_PALETTE_ORDERING[i] * 3, to = i * 3; j < 3; j += 1, from += 1, to += 1)
        {
            file->screen->palette->bytes[to] = full_palette[from];
        }
    }
    generate_rgb_data(file->screen->palette);
    file->screen->font    = load_8x16x256_font(file_ptr);
    image_bytes_length    = file->actual_file_size - (ARTWORX_VERSION_LENGTH + ARTWORX_PALETTE_LENGTH + ARTWORX_FONT_LENGTH);
    file->screen->columns = ARTWORX_DEFAULT_COLUMNS;
    file->screen->rows    = (uint16_t) (image_bytes_length / 2 / ARTWORX_DEFAULT_COLUMNS);
    load_screen(file->screen, file_ptr);
    fclose(file_ptr);
    return file;
}

void free_artworx_file(ArtworxFile *file)
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

void debug_artworx_file(ArtworxFile *file)
{
    debug_screen(file->screen);
    printf("Artworx actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* load_artworx_file_and_generate_canvas(char const *filename)
{
    ArtworxFile* file = load_artworx(filename);
    debug_artworx_file(file);
    Canvas *canvas = screen_to_canvas(file->screen);
    free_artworx_file(file);
    return canvas;
}