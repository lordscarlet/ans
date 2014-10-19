#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL.h>

#include "canvas.h"
#include "../file/formats/screen.h"

uint32_t TEXTURE_MAX_HEIGHT = 2048;

Canvas* create_canvas(uint32_t width, uint32_t height)
{
    Canvas *canvas = malloc(sizeof(Canvas));
    canvas->width = width;
    canvas->height = height;
    canvas->length = width * height * 3;
    if(canvas->length > 0)
    {
        canvas->data = malloc(canvas->length);
    }
    else
    {
        canvas->data = NULL;
    }
    return canvas;
}

void free_canvas(Canvas *canvas)
{
    if(canvas != NULL)
    {
        if(canvas->data != NULL)
        {
            free(canvas->data);
        }
        free(canvas);
    }
}

void debug_canvas(Canvas *canvas)
{
    printf("Image width: %i\n",          canvas->width);
    printf("Image height: %i\n",         canvas->height);
}

TextureCollection* create_textures(SDL_Renderer *renderer, Canvas *canvas)
{
    TextureCollection *textures = malloc(sizeof(TextureCollection));
    textures->canvas = canvas;
    textures->max_height = TEXTURE_MAX_HEIGHT;
    textures->final_height = textures->canvas->height % textures->max_height;
    if(textures->final_height == 0)
    {
        textures->final_height = textures->max_height;
    }
    textures->length = (uint32_t) ceill((double) canvas->height / (double) textures->max_height);
    if(textures->length > 0)
    {
        textures->data = malloc(sizeof(SDL_Texture*) * textures->length);
        for(uint32_t i = 0; i < textures->length; i += 1)
        {
            textures->data[i] = NULL;
        }
    }
    else
    {
        textures->data = NULL;
    }
    return textures;
}

void render_texture(SDL_Renderer *renderer, TextureCollection *textures, uint32_t i)
{
    uint32_t height;
    if(i == textures->length - 1)
    {
        height = textures->final_height;
    }
    else
    {
        height = textures->max_height;
    }
    textures->data[i] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_RENDERER_ACCELERATED, textures->canvas->width, height);
    SDL_UpdateTexture(textures->data[i], NULL, textures->canvas->data + textures->canvas->width * textures->max_height * 3 * i, textures->canvas->width * 3);
}

void free_textures(TextureCollection *textures)
{
    if(textures != NULL)
    {
        for(uint32_t i = 0; i < textures->length; i += 1)
        {
            if(textures->data[i] != NULL)
            {
                SDL_DestroyTexture(textures->data[i]);
            }
        }
        free(textures);
    }
}

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

Canvas* screen_to_canvas(Screen *screen)
{
    Canvas  *canvas = create_canvas(screen->columns * screen->font->width, screen->rows * screen->font->height);
    uint8_t ascii_code, foreground, background;
    uint8_t *draw_foreground, *draw_background;
    switch(screen->type)
    {
        case CHARACTERS:
        for(uint32_t y = 0, i = 0; y < screen->rows; y += 1)
        {
            for(uint32_t x = 0; x < screen->columns; x += 1, i += 1)
            {
                ascii_code = screen->data[i];
                draw_glyph(canvas, ascii_code, 1, 0, x, y, screen->palette, screen->font);
            }
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        for(uint32_t y = 0, i = 0; y < screen->rows; y += 1)
        {
            for(uint32_t x = 0; x < screen->columns; x += 1, i += 2)
            {
                ascii_code = screen->data[i];
                foreground = screen->data[i + 1] & 0xf;
                background = screen->data[i + 1] >> 4;
                if(!screen->non_blink && background >= 8)
                {
                    background -= 8;
                }
                draw_glyph(canvas, ascii_code, foreground, background, x, y, screen->palette, screen->font);
            }
        }
        break;
        case RGB_DATA:
        for(uint32_t y = 0, i = 0; y < screen->rows; y += 1)
        {
            for(uint32_t x = 0; x < screen->columns; x += 1, i += 7)
            {
                ascii_code = screen->data[i];
                draw_rgb_glyph(canvas, ascii_code, screen->data + i + 1, screen->data + i + 4, x, y, screen->font);
            }
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        for(uint32_t y = 0, i = 0; y < screen->rows; y += 1)
        {
            for(uint32_t x = 0; x < screen->columns; x += 1, i += 10)
            {
                ascii_code = screen->data[i];
                foreground = screen->data[i + 1] & 0xf;
                background = screen->data[i + 1] >> 4;
                if(!screen->non_blink && background >= 8)
                {
                    background -= 8;
                }
                if(screen->data[i + 2] == NON_RGB_ATTRIBUTE_DATA)
                {
                    draw_foreground = screen->palette->rgb_bytes + foreground * 3;
                }
                else
                {
                    draw_foreground = screen->data + i + 3;
                }
                if(screen->data[i + 6] == NON_RGB_ATTRIBUTE_DATA)
                {
                    draw_background = screen->palette->rgb_bytes + background * 3;
                }
                else
                {
                    draw_background = screen->data + i + 7;
                }
                draw_rgb_glyph(canvas, ascii_code, draw_foreground, draw_background, x, y, screen->font);
            }
        }
        break;
    }
    canvas->font_height = screen->font->height;
    return canvas;
}
