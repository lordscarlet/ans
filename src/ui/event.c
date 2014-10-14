#include <stdbool.h>
#include <SDL.h>
#include <time.h>

#include "../image/canvas.h"

uint32_t MAX_TEXTURES = 8;

void draw_textures(size_t width, size_t height, SDL_Renderer *renderer, TextureCollection* textures, int32_t *y_pos)
{
    SDL_Rect src_rect, dst_rect;
    if(*y_pos < 0)
    {
        *y_pos = 0;
    }
    else if(*y_pos > textures->canvas->height - height)
    {
        *y_pos = textures->canvas->height - height;
    }
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = textures->canvas->width;
    src_rect.h = textures->max_height;
    dst_rect.x = (width - textures->canvas->width) / 2;
    dst_rect.y = (int) -(*y_pos % textures->max_height);
    dst_rect.w = textures->canvas->width;
    dst_rect.h = textures->max_height;
    SDL_RenderClear(renderer);
    for(uint32_t i = *y_pos / textures->max_height; i < textures->length; i += 1)
    {
        if(i >= MAX_TEXTURES)
        {
            if(textures->data[i - MAX_TEXTURES] != NULL)
            {
                SDL_DestroyTexture(textures->data[i - MAX_TEXTURES]);
                textures->data[i - MAX_TEXTURES] = NULL;
            }
        }
        if(i + MAX_TEXTURES < textures->length)
        {
            if(textures->data[i + MAX_TEXTURES] != NULL)
            {
                SDL_DestroyTexture(textures->data[i + MAX_TEXTURES]);
                textures->data[i + MAX_TEXTURES] = NULL;
            }
        }
        if(textures->data[i] == NULL)
        {
            render_texture(renderer, textures, i);
        }
        if(i == textures->length - 1)
        {
            src_rect.h = textures->final_height;
            dst_rect.h = textures->final_height;
        }
        SDL_RenderCopy(renderer, textures->data[i], &src_rect, &dst_rect);
        dst_rect.y += textures->max_height;
        if(dst_rect.y > height)
        {
            break;
        }
    }
    SDL_RenderPresent(renderer);
}

void joy_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, SDL_Joystick *joystick, int32_t *y_pos)
{
    int32_t y_joy;
    SDL_Event event;
    while(true) {
        SDL_PollEvent(&event);
        y_joy = SDL_JoystickGetAxis(joystick, 1) / 4095;
        if(y_joy > 2)
        {
            *y_pos += y_joy - 2;
            draw_textures(width, height, renderer, textures, y_pos);
        }
        else if (y_joy < -2)
        {
            *y_pos += y_joy + 2;
            draw_textures(width, height, renderer, textures, y_pos);
        }
        else
        {
            break;
        }
    }
}

void key_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, SDL_KeyboardEvent *event, int32_t *y_pos, bool *quit)
{
    
    bool shift = (event->keysym.mod & KMOD_LSHIFT) || (event->keysym.mod & KMOD_RSHIFT);
    switch(event->keysym.sym)
    {
        case SDLK_DOWN:
        if(shift)
        {
            *y_pos += textures->canvas->font_height * 4;
        }
        else
        {
            *y_pos += textures->canvas->font_height;
        }
        draw_textures(width, height, renderer, textures, y_pos);
        break;
        case SDLK_UP:
        if(shift)
        {
            *y_pos -= textures->canvas->font_height * 4;
        }
        else
        {
            *y_pos -= textures->canvas->font_height;
        }
        draw_textures(width, height, renderer, textures, y_pos);
        break;
        case SDLK_SPACE:
        if(shift)
        {
            *y_pos -= height;
        }
        else
        {
            *y_pos += height;
        }
        draw_textures(width, height, renderer, textures, y_pos);
        break;
        case SDLK_ESCAPE:
        *quit = true;
        break;
    }
}

void generate_initial_textures(SDL_Renderer *renderer, TextureCollection* textures)
{
    uint32_t max;
    if(textures->length < MAX_TEXTURES)
    {
        max = textures->length;
    }
    else
    {
        max = MAX_TEXTURES;
    }
    for(uint32_t i = 0; i < max; i += 1)
    {
        render_texture(renderer, textures, i);
    }
}

void event_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas)
{
    TextureCollection* textures = create_textures(renderer, canvas);
    SDL_Joystick *joystick = SDL_JoystickOpen(0);
    SDL_Event event;
    bool quit = false;
    int32_t y_pos = 0;
    generate_initial_textures(renderer, textures);
    draw_textures(width, height, renderer, textures, &y_pos);
    while(!quit)
    {
        SDL_WaitEventTimeout(&event, 50);
        switch(event.type)
        {
            case SDL_QUIT:
            quit = true;
            break;
            case SDL_JOYAXISMOTION:
            joy_loop(width, height, renderer, textures, joystick, &y_pos);
            break;
            case SDL_JOYBUTTONDOWN:
            quit = true;
            break;
            case SDL_KEYDOWN:
            key_event(width, height, renderer, textures, &event.key, &y_pos, &quit);
            break;
            
        }
    }
    free_textures(textures);
}
