#include <stdio.h>
#include <stdlib.h>

#include "xbin.h"
#include "canvas.h"
#include "renderer.h"

enum Flags
{
    FLAG_PALETTE   = 1,
    FLAG_FONT      = 2,
    FLAG_COMPRESS  = 4,
    FLAG_NON_BLINK = 8,
    FLAG_CHAR_512  = 16
};

XBin_File* load_xbin(char const *filename)
{
    char id[5];
    uint8_t eof, flags;
    bool flag_palette, flag_font, flag_compress;
    FILE *file_ptr = fopen(filename, "r");
    XBin_File *file = malloc(sizeof(XBin_File));
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
        file->palette_bytes = malloc(48);
        fread(file->palette_bytes, 1, 48, file_ptr);
    }
    else
    {
        file->palette_bytes = NULL;
    }
    if(flag_font)
    {
        if(file->flag_char_512)
        {
            file->font_bytes_length = 512 * file->font_height;
        }
        else
        {
            file->font_bytes_length = 256 * file->font_height;
        }
        file->font_bytes = malloc(file->font_bytes_length);
        fread(file->font_bytes, 1, file->font_bytes_length, file_ptr);
    }
    else
    {
        file->font_bytes_length = 0;
        file->font_bytes = NULL;
    }
    if(file->columns > 0 && file->rows > 0)
    {
        if(!flag_compress)
        {
            file->image_bytes_length = file->columns * file->rows * 2;
            file->image_bytes = malloc(file->image_bytes_length);
            fread(file->image_bytes, 1, file->image_bytes_length, file_ptr);
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

void free_xbin_file(XBin_File *file)
{
    if(file != NULL)
    {
        if(file->palette_bytes != NULL)
        {
            free(file->palette_bytes);
        }
        if(file->font_bytes != NULL)
        {
            free(file->font_bytes);
        }
        if(file->image_bytes != NULL)
        {
            free(file->image_bytes);
        }
        free(file);
    }
}

void debug_xbin_file(XBin_File *file)
{
    printf("XBin columns: %i\n",             file->columns);
    printf("XBin rows: %i\n",                file->rows);
    printf("XBin font height: %i\n",         file->font_height);
    printf("XBin 512 Characters: ");
    if(file->flag_char_512)
    {
        printf("Yes\n");
    }
    else
    {
        printf("No\n");
    }
    if(file->palette_bytes != NULL)
    {
        printf("XBin palette: ");
        for(size_t i = 0; i < 48; i += 3) {
            printf("(%d, %d, %d)", file->palette_bytes[i], file->palette_bytes[i + 1], file->palette_bytes[i + 2]);
            if(i < 45)
            {
                printf(", ");
            }
        }
        printf("\n");
    }
    if(file->font_bytes != NULL)
    {
        printf("XBin font length (bytes): %d\n", file->font_bytes_length);
    }
    if(file->image_bytes != NULL)
    {
        printf("XBin image length (bytes): %d\n", file->image_bytes_length);
    }
}

Canvas* xbin_file_to_canvas(XBin_File *file)
{
    Canvas *canvas = create_canvas(file->columns * 8, file->rows * file->font_height);
    uint8_t palette_rgb[48];
    uint8_t font_bits[8 * file->font_height * 256];
    uint8_t ascii_code, foreground, background;
    convert_palette(file->palette_bytes, palette_rgb);
    font_bytes_to_bits(file->font_bytes, file->font_height, font_bits);
    for(size_t y = 0, i = 0; y < file->rows; y += 1)
    {
        for(size_t x = 0; x < file->columns; x += 1, i += 2)
        {
            ascii_code = file->image_bytes[i];
            foreground = file->image_bytes[i + 1] & 0xf;
            background = file->image_bytes[i + 1] >> 4;
            draw_glyph(canvas, ascii_code, foreground, background, x, y, palette_rgb, font_bits, 8, file->font_height);
        }
    }
    canvas->font_height = file->font_height;
    return canvas;
}
