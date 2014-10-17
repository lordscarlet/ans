#include <stdlib.h>
#include <stdio.h>

#include "font.h"
#include "fonts/ibm_vga_8x16.h"

void generate_bits(Font *font)
{
    font->bits = malloc(font->width * font->height * font->length);
    for(uint32_t i = 0, k = 0; i < font->height * font->length; i += 1)
    {
        for(int8_t j = 7; j >= 0; j -= 1, k += 1)
        {
            font->bits[k] = (font->bytes[i] >> j) & 1;
        }
    }
}


Font* get_preset_font(FontType type)
{
    Font *font = malloc(sizeof(Font));
    font->type = type;
    switch(type)
    {
        case IBM_VGA_8x16:
        font->width  = ibm_vga_8x16_width;
        font->height = ibm_vga_8x16_height;
        font->length = ibm_vga_8x16_length;
        font->bytes  = ibm_vga_8x16;
        generate_bits(font);
        return font;
        default:
        free(font);
        return NULL;
    }
}

Font* load_font(uint8_t height, uint16_t length, FILE *file_ptr)
{
    Font *font   = malloc(sizeof(Font));
    font->type   = CUSTOM_FONT;
    font->width  = 8;
    font->height = height;
    font->length = length;
    font->bytes  = malloc(font->height * font->length);
    fread(font->bytes, 1, font->height * font->length, file_ptr);
    generate_bits(font);
    return font;
}

Font* load_8x16x256_font(FILE *file_ptr)
{
    return load_font(16, 256, file_ptr);
}

void free_font(Font *font)
{
    if(font != NULL)
    {
        if(font->type == CUSTOM_FONT)
        {
            free(font->bytes);
        }
        free(font->bits);
        free(font);
    }
}