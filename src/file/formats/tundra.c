#include <stdio.h>
#include <stdlib.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint32_t get_tundra_32_bit_number(FILE *file_ptr)
{
    uint8_t bytes[4];
    fread(bytes, 1, 4, file_ptr);
    return ((uint32_t) bytes[0] << 24) + ((uint32_t) bytes[1] << 16) + ((uint32_t) bytes[2] << 8) + (uint32_t) bytes[3];
}

TextArtFile* load_tundra_file(char const *filename)
{
    TextArtFile *file;
    FILE        *file_ptr;
    uint8_t     magic_number[9];
    uint8_t     ascii_code;
    uint8_t     *foreground = calloc(3, 1);
    uint8_t     *background = calloc(3, 1);
    file                    = malloc(sizeof(TextArtFile));
    file->screen            = create_screen_with_font(RGB_DATA, CP437_8x16);
    file->screen->non_blink = true;
    file_ptr                = fopen(filename, "r");
    file->sauce             = get_sauce(file_ptr);
    file->length            = get_real_file_size(file_ptr, file->sauce);
    file->screen->columns   = t_info_1(file->sauce, 80);
    fread(magic_number, 1, 9, file_ptr);
    for(uint16_t i = 9, x = 0, y = 0; i < file->length;)
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
