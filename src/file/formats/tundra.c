#include <stdio.h>
#include <stdlib.h>

#include "tundra.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"
#include "palette.h"
#include "font.h"

uint32_t get_tundra_32_bit_number(FILE *file_ptr)
{
    uint8_t bytes[4];
    fread(bytes, 1, 4, file_ptr);
    return ((uint32_t) bytes[0] << 24) + ((uint32_t) bytes[1] << 16) + ((uint32_t) bytes[2] << 8) + (uint32_t) bytes[3];
}

TundraFile* load_tundra(char const *filename)
{
    TundraFile *file;
    FILE       *file_ptr;
    uint8_t    magic_number[9];
    uint8_t    ascii_code;
    uint8_t    *foreground = calloc(3, 1);
    uint8_t    *background = calloc(3, 1);
    file                   = malloc(sizeof(TundraFile));
    file->screen           = create_screen_with_font(RGB_DATA, CP437_8x16);
    file_ptr               = fopen(filename, "r");
    file->sauce            = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    file->screen->columns  = t_info_1(file->sauce, 80);
    fread(magic_number, 1, 9, file_ptr);
    for(uint16_t i = 9, x = 0, y = 0; i < file->actual_file_size;)
    {
        if(x == file->screen->columns)
        {
            x = 0;
            y += 1;
        }
        fread(&ascii_code, 1, 1, file_ptr);
        i += 1;
        if(ascii_code == 1)
        {
            y = (uint16_t) get_tundra_32_bit_number(file_ptr);
            x = (uint16_t) get_tundra_32_bit_number(file_ptr);
            i += 8;
        }
        if(ascii_code == 2)
        {
            fread(&ascii_code, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(foreground, 1, 3, file_ptr);
            i += 5;
        }
        if(ascii_code == 4)
        {
            fread(&ascii_code, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(background, 1, 3, file_ptr);
            i += 5;
        }
        if(ascii_code == 6)
        {
            fread(&ascii_code, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(foreground, 1, 3, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(background, 1, 3, file_ptr);
            i += 9;
        }
        if (ascii_code != 1 && ascii_code != 2 && ascii_code != 4 && ascii_code != 6) {
            put_rgb_data_on_screen(file->screen, &x, &y, ascii_code, foreground, background);
        }
    }
    fclose(file_ptr);
    truncate_screen_data(file->screen);
    free(foreground);
    free(background);
    return file;
}

void free_tundra_file(TundraFile *file)
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

void debug_tundra_file(TundraFile *file)
{
    debug_screen(file->screen);
    printf("Tundra actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* load_tundra_file_and_generate_canvas(char const *filename)
{
    TundraFile* file = load_tundra(filename);
    debug_tundra_file(file);
    Canvas *canvas = screen_to_canvas(file->screen);
    free_tundra_file(file);
    return canvas;
}