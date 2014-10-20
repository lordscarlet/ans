#include <stdio.h>
#include <stdlib.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint32_t ICE_DRAW_FONT_SIZE        = 4096;
uint32_t ICE_DRAW_PALETTE_SIZE     = 48;
uint32_t ICE_DRAW_IMAGE_DATA_START = 12;
uint16_t ICE_DRAW_DEFAULT_COLUMNS  = 80;

TextArtFile* load_ice_draw_file(char *filename)
{
    TextArtFile *file;
    FILE        *file_ptr;
    uint32_t    image_data_end;
    uint16_t    c;
    uint16_t    uncompressed_data_size = 0;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint8_t     loop, ascii_code, attribute;
    file                    = malloc(sizeof(TextArtFile));
    file->screen            = create_screen(CHARACTER_AND_ATTRIBUTE_PAIR);
    file->screen->non_blink = true;
    file->screen->columns   = ICE_DRAW_DEFAULT_COLUMNS;
    file_ptr                = fopen(filename, "r");
    file->sauce             = get_sauce(file_ptr);
    file->length            = get_real_file_size(file_ptr, file->sauce);
    image_data_end          = file->length - (ICE_DRAW_FONT_SIZE + ICE_DRAW_PALETTE_SIZE);
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
    fseek(file_ptr, (long) file->length - (ICE_DRAW_FONT_SIZE + ICE_DRAW_PALETTE_SIZE), SEEK_SET);
    file->screen->font    = create_and_load_8x16x256_font(file_ptr);
    file->screen->palette = load_palette(file_ptr);
    fclose(file_ptr);
    return file;
}
