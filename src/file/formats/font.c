#include <stdlib.h>
#include <stdio.h>

#include "font.h"
#include "fonts/cp437_8x16.h"

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
        case CP437_8x16:
        font->width  = cp437_8x16_width;
        font->height = cp437_8x16_height;
        font->length = cp437_8x16_length;
        font->bytes  = cp437_8x16;
        font->name   = cp437_8x16_name;
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
    font->name   = NULL;
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

void debug_font(Font *font)
{
    printf("Font: ");
    if(font->type == CUSTOM_FONT)
    {
        printf("Included in file\n");
    }
    else
    {
        printf("%s\n", font->name);
    }
    printf("Font width: %d\n", font->width);
    printf("Font height: %d\n", font->height);
    printf("Font glyphs (amount): %d\n", font->length);
}
