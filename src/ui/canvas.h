#ifndef CANVAS_H_FC7B83B1
#define CANVAS_H_FC7B83B1

#include <stdint.h>
#include <SDL.h>

#include "../file/formats/screen.h"

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint8_t *data;
    uint32_t length;
    uint8_t font_height;
} Canvas;

typedef struct
{
    SDL_Texture **data;
    uint32_t width;
    uint32_t height;
    uint32_t length;
    uint32_t max_height;
    uint32_t final_height;
    Canvas *canvas;
} TextureCollection;

Canvas* create_canvas(uint32_t width, uint32_t height);

void free_canvas(Canvas *canvas);

TextureCollection* create_textures(SDL_Renderer *renderer, Canvas *canvas);

void render_texture(SDL_Renderer *renderer, TextureCollection *textures, uint32_t i);

void free_textures(TextureCollection *textures);

void debug_canvas(Canvas *canvas);

Canvas* screen_to_canvas(Screen *screen);

#endif /* end of include guard: CANVAS_H_FC7B83B1 */
