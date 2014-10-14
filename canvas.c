#include <stdlib.h>
#include <math.h>
#include <SDL.h>

#include "canvas.h"

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
    printf("Image length (bytes): %i\n", canvas->length);
}

TextureCollection* create_textures(SDL_Renderer *renderer, Canvas *canvas)
{
    TextureCollection *textures = malloc(sizeof(TextureCollection));
    textures->canvas = canvas;
    textures->max_height = TEXTURE_MAX_HEIGHT;
    textures->final_height = textures->canvas->height % textures->max_height;
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