#include <stdio.h>
#include <stdlib.h>

#include "icedraw.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"
#include "palette.h"
#include "font.h"

uint32_t ICE_DRAW_FONT_SIZE        = 4096;
uint32_t ICE_DRAW_PALETTE_SIZE     = 48;
uint32_t ICE_DRAW_IMAGE_DATA_START = 12;
uint16_t ICE_DRAW_DEFAULT_COLUMNS  = 80;

IceDrawFile* load_ice_draw(char const *filename)
{
    IceDrawFile *file;
    FILE        *file_ptr;
    uint32_t    image_data_end;
    uint16_t    c;
    uint16_t    uncompressed_data_size = 0;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint8_t     loop, ascii_code, attribute;
    file                   = malloc(sizeof(IceDrawFile));
    file->screen           = create_screen(CHARACTER_AND_ATTRIBUTE_PAIR);
    file->screen->columns  = ICE_DRAW_DEFAULT_COLUMNS;
    file_ptr               = fopen(filename, "r");
    file->sauce            = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    image_data_end         = file->actual_file_size - (ICE_DRAW_FONT_SIZE + ICE_DRAW_PALETTE_SIZE);
    fseek(file_ptr, (long) ICE_DRAW_IMAGE_DATA_START, SEEK_SET);
    for(uint32_t i = ICE_DRAW_IMAGE_DATA_START; i < image_data_end;)
    {
        fread(&ascii_code, 1, 1, file_ptr);
        fread(&attribute,  1, 1, file_ptr);
        i += 2;
        c = ((uint16_t) attribute << 8) + ascii_code;
        if(c == 1)
        {
            fread(&loop, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(&ascii_code, 1, 1, file_ptr);
            fread(&attribute,  1, 1, file_ptr);
            i += 4;
            for(size_t k = 0; k < loop; k += 1, uncompressed_data_size += 2)
            {
                put_character_and_attribute_pair_on_screen(file->screen, &x, &y, ascii_code, attribute);
            }
        }
        else
        {
            put_character_and_attribute_pair_on_screen(file->screen, &x, &y, ascii_code, attribute);
        }
    }
    truncate_screen_data(file->screen);
    fseek(file_ptr, (long) file->actual_file_size - (ICE_DRAW_FONT_SIZE + ICE_DRAW_PALETTE_SIZE), SEEK_SET);
    file->screen->font    = load_8x16x256_font(file_ptr);
    file->screen->palette = load_palette(file_ptr);
    fclose(file_ptr);
    return file;
}

void free_ice_draw_file(IceDrawFile *file)
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

void debug_ice_draw_file(IceDrawFile *file)
{
    debug_screen(file->screen);
    printf("Ice Draw actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* load_ice_draw_file_and_generate_canvas(char const *filename)
{
    IceDrawFile* file = load_ice_draw(filename);
    debug_ice_draw_file(file);
    Canvas *canvas = screen_to_canvas(file->screen);
    free_ice_draw_file(file);
    return canvas;
}