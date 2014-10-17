#include <stdio.h>
#include <stdlib.h>

#include "xbin.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "../../image/renderer.h"
#include "palette.h"
#include "font.h"

enum Flags
{
    FLAG_PALETTE   = 1,
    FLAG_FONT      = 2,
    FLAG_COMPRESS  = 4,
    FLAG_NON_BLINK = 8,
    FLAG_CHAR_512  = 16
};

void decompress(uint8_t *image_bytes, uint32_t image_bytes_length, FILE *file_ptr)
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

XBinFile* load_xbin(char const *filename)
{
    char id[5];
    uint8_t eof, flags;
    bool flag_palette, flag_font, flag_compress;
    FILE *file_ptr = fopen(filename, "r");
    XBinFile *file = malloc(sizeof(XBinFile));
    file->sauce = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    fread(id,                 1, 4, file_ptr);
    id[4] = 0;
    fread(&eof,               1, 1, file_ptr);
    fread(&file->columns,     2, 1, file_ptr);
    fread(&file->rows,        2, 1, file_ptr);
    fread(&file->font_height, 1, 1, file_ptr);
    fread(&flags,             1, 1, file_ptr);
    flag_palette         = (flags & FLAG_PALETTE)   == FLAG_PALETTE;
    flag_font            = (flags & FLAG_FONT)      == FLAG_FONT;
    flag_compress        = (flags & FLAG_COMPRESS)  == FLAG_COMPRESS;
    file->flag_non_blink = (flags & FLAG_NON_BLINK) == FLAG_NON_BLINK;
    file->flag_char_512  = (flags & FLAG_CHAR_512)  == FLAG_CHAR_512;
    if(flag_palette)
    {
        file->palette = load_palette(file_ptr);
    }
    else
    {
        file->palette = get_preset_palette(BINARY_PALETTE);
    }
    if(flag_font)
    {
        if(file->flag_char_512)
        {
            file->font = load_font(file->font_height, 512, file_ptr);
        }
        else
        {
            file->font = load_font(file->font_height, 256, file_ptr);
        }
    }
    else
    {
        file->font = get_preset_font(IBM_VGA_8x16);
    }
    if(file->columns > 0 && file->rows > 0)
    {
        file->image_bytes_length = file->columns * file->rows * 2;
        file->image_bytes = malloc(file->image_bytes_length);
        if(!flag_compress)
        {
            fread(file->image_bytes, 1, file->image_bytes_length, file_ptr);
        }
        else
        {
            decompress(file->image_bytes, file->image_bytes_length, file_ptr);
        }
    }
    else
    {
        file->image_bytes_length = 0;
        file->image_bytes = NULL;
    }
    fclose(file_ptr);
    return file;
}

void free_xbin_file(XBinFile *file)
{
    if(file != NULL)
    {
        if(file->image_bytes != NULL)
        {
            free(file->image_bytes);
        }
        if(file->sauce != NULL)
        {
            free(file->sauce);
        }
        free_palette(file->palette);
        free_font(file->font);
        free(file);
    }
}

void debug_xbin_file(XBinFile *file)
{
    printf("XBin columns: %i\n",             file->columns);
    printf("XBin rows: %i\n",                file->rows);
    printf("XBin 512 Characters: ");
    if(file->flag_char_512)
    {
        printf("Yes\n");
    }
    else
    {
        printf("No\n");
    }
    printf("XBin palette: ");
    if(file->palette->type == CUSTOM_PALETTE)
    {
        printf("Included in file\n");
        printf("XBin palette values: ");
        for(size_t i = 0; i < 48; i += 3) {
            printf("(%d, %d, %d)", file->palette->bytes[i], file->palette->bytes[i + 1], file->palette->bytes[i + 2]);
            if(i < 45)
            {
                printf(", ");
            }
        }
        printf("\n");
    }
    else
    {
        printf("None included\n");
    }
    printf("XBin font: ");
    if(file->font->type == CUSTOM_FONT)
    {
        printf("Included in file\n");
        printf("XBin font height: %i\n", file->font_height);
    }
    else
    {
        printf("None included\n");
    }
    if(file->image_bytes != NULL)
    {
        printf("XBin image length (bytes): %d\n", file->image_bytes_length);
    }
    printf("XBin actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* xbin_file_to_canvas(XBinFile *file)
{
    Canvas  *canvas = create_canvas(file->columns * file->font->width, file->rows * file->font->height);
    uint8_t ascii_code, foreground, background;
    for(uint32_t y = 0, i = 0; y < file->rows; y += 1)
    {
        for(uint32_t x = 0; x < file->columns; x += 1, i += 2)
        {
            ascii_code = file->image_bytes[i];
            foreground = file->image_bytes[i + 1] & 0xf;
            background = file->image_bytes[i + 1] >> 4;
            if(!file->flag_non_blink && background >= 8)
            {
                background -= 8;
            }
            draw_glyph(canvas, ascii_code, foreground, background, x, y, file->palette, file->font);
        }
    }
    canvas->font_height = file->font->height;
    return canvas;
}

Canvas* load_xbin_file_and_generate_canvas(char const *filename)
{
    XBinFile* file = load_xbin(filename);
    debug_xbin_file(file);
    Canvas *canvas = xbin_file_to_canvas(file);
    free_xbin_file(file);
    return canvas;
}