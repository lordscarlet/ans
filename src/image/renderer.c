#include <stdbool.h>
#include <SDL.h>

#include "canvas.h"

void convert_palette(uint8_t *palette_bytes, uint8_t *palette_rgb)
{
    for(size_t i = 0; i < 48; i += 1)
    {
        palette_rgb[i] = palette_bytes[i] << 2 | palette_bytes[i] >> 4;
    }
}

void font_bytes_to_bits(uint8_t *font_bytes, uint8_t font_height, uint8_t *font_bits)
{
    /*
        TODO deal with 512 character xbins
    */
    for(size_t i = 0, k = 0; i < font_height * 256; i += 1)
    {
        for(int8_t j = 7; j >= 0; j -= 1, k += 1)
        {
            font_bits[k] = (font_bytes[i] >> j) & 1;
        }
    }
}

void draw_glyph(Canvas *canvas, uint8_t ascii_code, uint8_t foreground, uint8_t background, uint16_t x, uint16_t y, uint8_t *palette_rgb, uint8_t *font_bits, uint8_t font_width, uint8_t font_height)
{
    uint32_t ascii_code_pos        = ascii_code * font_width * font_height;
    uint8_t palette_foreground_pos = foreground * 3;
    uint8_t palette_background_pos = background * 3;
    uint8_t palette_pos;
    for(size_t font_y = 0, i = (y * font_height * canvas->width + x * font_width) * 3; font_y < font_height; font_y += 1)
    {
        for(size_t font_x = 0; font_x < font_width; font_x += 1, i += 3, ascii_code_pos += 1)
        {
            if(font_bits[ascii_code_pos] == 1)
            {
                palette_pos = palette_foreground_pos;
            }
            else
            {
                palette_pos = palette_background_pos;
            }
            canvas->data[i + 0] = palette_rgb[palette_pos + 0];
            canvas->data[i + 1] = palette_rgb[palette_pos + 1];
            canvas->data[i + 2] = palette_rgb[palette_pos + 2];
        }
        i += (canvas->width - font_width) * 3;
    }
}
