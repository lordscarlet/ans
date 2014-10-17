#include <stdbool.h>
#include <SDL.h>

#include "canvas.h"
#include "../file/formats/palette.h"
#include "../file/formats/font.h"

void draw_rgb_glyph(Canvas *canvas, uint8_t ascii_code, uint8_t *foreground, uint8_t *background, uint16_t x, uint16_t y, Font *font)
{
    uint32_t ascii_code_pos = ascii_code * font->width * font->height;
    for(size_t font_y = 0, i = (y * font->height * canvas->width + x * font->width) * 3; font_y < font->height; font_y += 1)
    {
        for(size_t font_x = 0; font_x < font->width; font_x += 1, i += 3, ascii_code_pos += 1)
        {
            if(font->bits[ascii_code_pos] == 1)
            {
                memcpy(canvas->data + i, foreground, 3);
            }
            else
            {
                memcpy(canvas->data + i, background, 3);
            }
        }
        i += (canvas->width - font->width) * 3;
    }
}

void draw_glyph(Canvas *canvas, uint8_t ascii_code, uint8_t foreground, uint8_t background, uint16_t x, uint16_t y, Palette *palette, Font *font)
{
    uint32_t ascii_code_pos         = ascii_code * font->width * font->height;
    uint8_t  palette_foreground_pos = foreground * 3;
    uint8_t  palette_background_pos = background * 3;
    for(size_t font_y = 0, i = (y * font->height * canvas->width + x * font->width) * 3; font_y < font->height; font_y += 1)
    {
        for(size_t font_x = 0; font_x < font->width; font_x += 1, i += 3, ascii_code_pos += 1)
        {
            if(font->bits[ascii_code_pos] == 1)
            {
                memcpy(canvas->data + i, palette->rgb_bytes + palette_foreground_pos, 3);
            }
            else
            {
                memcpy(canvas->data + i, palette->rgb_bytes + palette_background_pos, 3);
            }
        }
        i += (canvas->width - font->width) * 3;
    }
}
