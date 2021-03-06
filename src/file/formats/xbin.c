#include <stdio.h>
#include <stdlib.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint16_t XBIN_PALETTE_LENGTH = 16;

enum Flags
{
    FLAG_PALETTE   = 1,
    FLAG_FONT      = 2,
    FLAG_COMPRESS  = 4,
    FLAG_NON_BLINK = 8,
    FLAG_CHAR_512  = 16
};

void decompress_xbin(uint8_t *image_bytes, uint32_t image_bytes_length, FILE *file_ptr)
{
    uint8_t p, count, repeat_char, repeat_attr;
    for(uint32_t i = 0; i < image_bytes_length;)
    {
        fread(&p, 1, 1, file_ptr);
        count = p & 0x3f;
        switch(p >> 6)
        {
            case 1:
            fread(&repeat_char, 1, 1, file_ptr);
            for(uint8_t j = 0; j <= count; j += 1, i += 2)
            {
                image_bytes[i + 0] = repeat_char;
                fread(image_bytes + i + 1, 1, 1, file_ptr);
            }
            break;
            case 2:
            fread(&repeat_attr, 1, 1, file_ptr);
            for(uint8_t j = 0; j <= count; j += 1, i += 2)
            {
                fread(image_bytes + i + 0, 1, 1, file_ptr);
                image_bytes[i + 1] = repeat_attr;
            }
            break;
            case 3:
            fread(&repeat_char, 1, 1, file_ptr);
            fread(&repeat_attr, 1, 1, file_ptr);
            for(uint8_t j = 0; j <= count; j += 1, i += 2)
            {
                image_bytes[i + 0] = repeat_char;
                image_bytes[i + 1] = repeat_attr;
            }
            break;
            default:
            for(uint8_t j = 0; j <= count; j += 1, i += 2)
            {
                fread(image_bytes + i + 0, 1, 1, file_ptr);
                fread(image_bytes + i + 1, 1, 1, file_ptr);
            }
        }
    }
}

TextArtFile* load_xbin_file(char *filename)
{
    char        id[5];
    uint8_t     eof, font_height, flags;
    bool        flag_palette, flag_font, flag_compress, flag_char_512;
    FILE        *file_ptr;
    TextArtFile *file;
    uint32_t    image_bytes_length;
    file_ptr               = fopen(filename, "r");
    file                   = malloc(sizeof(TextArtFile));
    file->screen           = create_screen(CHARACTER_AND_ATTRIBUTE_PAIR);
    file->sauce            = get_sauce(file_ptr);
    file->length           = get_real_file_size(file_ptr, file->sauce);
    id[4] = 0;
    fread(id,                     1, 4, file_ptr);
    fread(&eof,                   1, 1, file_ptr);
    fread(&file->screen->columns, 2, 1, file_ptr);
    fread(&file->screen->rows,    2, 1, file_ptr);
    fread(&font_height,           1, 1, file_ptr);
    fread(&flags,                 1, 1, file_ptr);
    flag_palette                      = (flags & FLAG_PALETTE)   == FLAG_PALETTE;
    flag_font                         = (flags & FLAG_FONT)      == FLAG_FONT;
    flag_compress                     = (flags & FLAG_COMPRESS)  == FLAG_COMPRESS;
    file->screen->non_blink           = (flags & FLAG_NON_BLINK) == FLAG_NON_BLINK;
    flag_char_512                     = (flags & FLAG_CHAR_512)  == FLAG_CHAR_512;
    file->screen->letter_spacing      = false;
    file->screen->legacy_aspect_ratio = false;
    if(flag_palette)
    {
        file->screen->palette = load_palette(file_ptr, XBIN_PALETTE_LENGTH, PAL_DATA_18BIT);
    }
    else
    {
        file->screen->palette = get_preset_palette(BINARY_PALETTE);
    }
    if(flag_font)
    {
        if(flag_char_512)
        {
            file->screen->font = create_and_load_font(font_height, 512, file_ptr);
        }
        else
        {
            file->screen->font = create_and_load_font(font_height, 256, file_ptr);
        }
    }
    else
    {
        file->screen->font = get_preset_font(CP437_8x16);
    }
    if(file->screen->columns > 0 && file->screen->rows > 0)
    {
        if(!flag_compress)
        {
            load_screen(file->screen, file_ptr);
        }
        else
        {
            image_bytes_length = file->screen->columns * file->screen->rows * 2;
            file->screen->data = malloc(image_bytes_length);
            decompress_xbin(file->screen->data, image_bytes_length, file_ptr);
        }
    }
    else
    {
        file->screen = NULL;
    }
    fclose(file_ptr);
    return file;
}
