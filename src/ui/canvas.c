#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <SDL.h>

#include "canvas.h"
#include "../file/file.h"
#include "../file/sauce.h"
#include "../file/formats/screen.h"
#include "../file/formats/palette.h"
#include "../file/formats/font.h"

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
    canvas->file = NULL;
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
        if(canvas->file != NULL)
        {
            free_text_art_file(canvas->file);
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
    textures->width = canvas->width;
    textures->height = canvas->height;
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
                memcpy(canvas->data + i, palette->rgb_data + palette_foreground_pos, 3);
            }
            else
            {
                memcpy(canvas->data + i, palette->rgb_data + palette_background_pos, 3);
            }
        }
        i += (canvas->width - font->width) * 3;
    }
}

void draw_box(Canvas *canvas, Font *font, Palette *palette, uint8_t foreground, uint8_t background)
{
    uint16_t columns = canvas->width / font->width;
    uint16_t rows = canvas->height / font->height;
    draw_glyph(canvas, 201, foreground, background, 0, 0, palette, font);
    draw_glyph(canvas, 200, foreground, background, 0, rows - 1, palette, font);
    draw_glyph(canvas, 187, foreground, background, columns - 1, 0, palette, font);
    draw_glyph(canvas, 187, foreground, background, columns - 1, 0, palette, font);
    draw_glyph(canvas, 188, foreground, background, columns - 1, rows - 1, palette, font);
    for(uint16_t x = 1; x < columns - 1; x += 1)
    {
        draw_glyph(canvas, 205, foreground, background, x, 0, palette, font);
        draw_glyph(canvas, 205, foreground, background, x, rows - 1, palette, font);
    }
    for(uint16_t y = 1; y < rows - 1; y += 1)
    {
        draw_glyph(canvas, 186, foreground, background, 0, y, palette, font);
        draw_glyph(canvas, 186, foreground, background, columns - 1, y, palette, font);
    }
    for(uint16_t y = 1; y < rows - 1; y += 1)
    {
        for(uint16_t x = 1; x < columns - 1; x += 1)
        {
            draw_glyph(canvas, 219, background, 0, x, y, palette, font);
        }
    }
}

void draw_text(Canvas *canvas, char *text, size_t length, uint8_t foreground, uint8_t background, uint16_t x, uint16_t y, Palette *palette, Font *font)
{
    for(size_t i = 0; i < length; i += 1)
    {
        draw_glyph(canvas, (uint8_t) text[i], foreground, background, x + i, y, palette, font);
    }
}

void draw_number(Canvas *canvas, int64_t number, uint8_t foreground, uint8_t background, uint16_t x, uint16_t y, Palette *palette, Font *font)
{
    char string[21];
    int length = sprintf(string, "%lld", number);
    for(size_t i = 0; i < length; i += 1)
    {
        draw_glyph(canvas, (uint8_t) string[i], foreground, background, x + i, y, palette, font);
    }
}

SDL_Texture* create_title_texture(SDL_Renderer *renderer, uint8_t *string, Sauce *sauce, Palette *palette, Font *font)
{
    SDL_Texture *texture;
    size_t  string_length;
    Canvas *canvas;
    char *title = get_title(sauce);
    if(title == NULL)
    {
        return NULL;
    }
    string_length = strlen((char*) title);
    canvas = create_canvas((string_length + 4) * font->width, font->height * 3);
    draw_box(canvas, font, palette, 15, 3);
    draw_text(canvas, title, string_length, 15, 3, 2, 1, palette, font);
    free(title);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_RENDERER_ACCELERATED, canvas->width, canvas->height);
    SDL_UpdateTexture(texture, NULL, canvas->data, canvas->width * 3);
    free_canvas(canvas);
    return texture;
}

SDL_Texture* create_sauce_texture(SDL_Renderer *renderer, Sauce *sauce, Palette *palette, Font *font)
{
    SDL_Texture *texture;
    Canvas *canvas;
    if(sauce == NULL)
    {
        canvas = create_canvas(font->width * 25, font->height * 3);
        draw_box(canvas, font, palette, 0, 7);
        draw_text(canvas, "No Sauce Record Found", 21, 0, 7, 2, 1, palette, font);
    }
    else
    {
        canvas = create_canvas(font->width * 49, font->height * 17);
        draw_box(canvas, font, palette, 0, 7);
        draw_text(canvas, "   Title:", 9, 0, 7, 2, 1, palette, font);
        draw_text(canvas, "  Author:", 9, 0, 7, 2, 2, palette, font);
        draw_text(canvas, "   Group:", 9, 0, 7, 2, 3, palette, font);
        draw_text(canvas, "    Date:", 9, 0, 7, 2, 4, palette, font);
        draw_text(canvas, "Filesize:", 9, 0, 7, 2, 5, palette, font);
        draw_text(canvas, "Datatype:", 9, 0, 7, 2, 6, palette, font);
        draw_text(canvas, "Filetype:", 9, 0, 7, 2, 7, palette, font);
        draw_text(canvas, "  TInfo1:", 9, 0, 7, 2, 8, palette, font);
        draw_text(canvas, "  TInfo2:", 9, 0, 7, 2, 9, palette, font);
        draw_text(canvas, "  TInfo3:", 9, 0, 7, 2, 10, palette, font);
        draw_text(canvas, "  TInfo4:", 9, 0, 7, 2, 11, palette, font);
        draw_text(canvas, "NonBlink:", 9, 0, 7, 2, 12, palette, font);
        draw_text(canvas, " Spacing:", 9, 0, 7, 2, 13, palette, font);
        draw_text(canvas, "As.Ratio:", 9, 0, 7, 2, 14, palette, font);
        draw_text(canvas, "  TInfoS:", 9, 0, 7, 2, 15, palette, font);
        draw_text(canvas, sauce->title, 35, 0, 7, 12, 1, palette, font);
        draw_text(canvas, sauce->author, 20, 0, 7, 12, 2, palette, font);
        draw_text(canvas, sauce->group, 20, 0, 7, 12, 3, palette, font);
        draw_text(canvas, sauce->date, 8, 0, 7, 12, 4, palette, font);
        draw_number(canvas, sauce->file_size, 0, 7, 12, 5, palette, font);
        draw_number(canvas, sauce->data_type, 0, 7, 12, 6, palette, font);
        draw_number(canvas, sauce->file_type, 0, 7, 12, 7, palette, font);
        draw_number(canvas, sauce->t_info_1, 0, 7, 12, 8, palette, font);
        draw_number(canvas, sauce->t_info_2, 0, 7, 12, 9, palette, font);
        draw_number(canvas, sauce->t_info_3, 0, 7, 12, 10, palette, font);
        draw_number(canvas, sauce->t_info_4, 0, 7, 12, 11, palette, font);
        if(sauce->non_blink)
        {
            draw_text(canvas, "On", 2, 0, 7, 12, 12, palette, font);
        }
        else
        {
            draw_text(canvas, "Off", 3, 0, 7, 12, 12, palette, font);
        }
        switch(sauce->letter_spacing)
        {
            case NO_LETTER_SPACE_PREFERENCE:
            draw_text(canvas, "No Preference", 13, 0, 7, 12, 13, palette, font);
            break;
            case EIGHT_PIXEL:
            draw_text(canvas, "8 Pixel Font", 12, 0, 7, 12, 13, palette, font);
            break;
            case NINE_PIXEL:
            draw_text(canvas, "9 Pixel Font", 12, 0, 7, 12, 13, palette, font);
            break;
            case INVALID_LETTER_SPACE:
            draw_text(canvas, "Invalid value", 13, 0, 7, 12, 13, palette, font);
            break;
        }
        switch(sauce->aspect_ratio)
        {
            case NO_ASPECT_RATIO_PREFERENCE:
            draw_text(canvas, "No Preference", 13, 0, 7, 12, 14, palette, font);
            break;
            case LEGACY:
            draw_text(canvas, "Legacy Device", 13, 0, 7, 12, 14, palette, font);
            break;
            case MODERN:
            draw_text(canvas, "Modern Device", 13, 0, 7, 12, 14, palette, font);
            break;
            case INVALID_ASPECT_RATIO:
            draw_text(canvas, "Invalid value", 13, 0, 7, 12, 14, palette, font);
            break;
        }
        draw_text(canvas, sauce->t_info_s, 22, 0, 7, 12, 15, palette, font);
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_RENDERER_ACCELERATED, canvas->width, canvas->height);
    SDL_UpdateTexture(texture, NULL, canvas->data, canvas->width * 3);
    free_canvas(canvas);
    return texture;
}

void draw_truncated_text(Canvas *canvas, char *text, uint16_t x, uint16_t y, size_t max_length, uint8_t foreground, uint8_t background, Palette *palette, Font *font)
{
    size_t string_length = strlen(text);
    if(string_length > max_length)
    {
        draw_text(canvas, "...", 3, foreground, background, x, y, palette, font);
        draw_text(canvas, text + string_length - max_length + 3, max_length - 3, foreground, background, x + 3, y, palette, font);
    }
    else
    {
        draw_text(canvas, text, string_length, foreground, background, x, y, palette, font);
    }
}

SDL_Texture* create_filename_list_texture(uint32_t height, SDL_Renderer *renderer, char **filenames, uint32_t filenames_length, uint16_t current_filename_index, Palette *palette, Font *font)
{
    SDL_Texture *texture;
    size_t box_width = 44;
    size_t box_height = (height / font->height) - 2;
    size_t list_width = box_width - 4;
    size_t list_height = box_height - 2;
    uint8_t foreground = 15;
    uint8_t background = 4;
    size_t start_index = 0;
    if(filenames_length < list_height)
    {
        list_height = filenames_length;
        box_height = list_height + 2;
    }
    Canvas *canvas;
    canvas = create_canvas(font->width * box_width, font->height * box_height);
    draw_box(canvas, font, palette, foreground, background);
    if(current_filename_index > list_height / 2)
    {
        if(filenames_length - current_filename_index + list_height / 2 < list_height)
        {
            start_index = filenames_length - list_height;
        }
        else
        {
            start_index = current_filename_index - list_height / 2;
        }
    }
    for(size_t i = start_index, y = 0; y < list_height; i += 1, y += 1)
    {
        if(i == current_filename_index)
        {
            draw_truncated_text(canvas, filenames[i], 2, 1 + y, list_width, background, foreground, palette, font);
        }
        else
        {
            draw_truncated_text(canvas, filenames[i], 2, 1 + y, list_width, foreground, background, palette, font);
        }
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_RENDERER_ACCELERATED, canvas->width, canvas->height);
    SDL_UpdateTexture(texture, NULL, canvas->data, canvas->width * 3);
    free_canvas(canvas);
    return texture;
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
                    draw_foreground = screen->palette->rgb_data + foreground * 3;
                }
                else
                {
                    draw_foreground = screen->data + i + 3;
                }
                if(screen->data[i + 6] == NON_RGB_ATTRIBUTE_DATA)
                {
                    draw_background = screen->palette->rgb_data + background * 3;
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
    return canvas;
}
