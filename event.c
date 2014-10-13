#include <stdbool.h>
#include <SDL.h>
#include <time.h>
#include "canvas.h"

void update_texture(size_t width, SDL_Renderer *renderer, int64_t *y_pos, SDL_Rect *dst_rect, SDL_Texture *texture, Canvas *canvas, size_t height)
{
    if(*y_pos < 0)
    {
        *y_pos = 0;
    }
    else if(*y_pos > canvas->height - height)
    {
        *y_pos = canvas->height - height;
    }
    SDL_UpdateTexture(texture, NULL, canvas->data + *y_pos * canvas->width * 3, canvas->width * 3);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, dst_rect);
    SDL_RenderPresent(renderer);
}

void event_loop(size_t width, size_t height, SDL_Renderer *renderer, Canvas *canvas, uint8_t font_height)
{
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_RENDERER_ACCELERATED, canvas->width, height);
    SDL_Joystick *joystick;
    joystick = SDL_JoystickOpen(0);
    int64_t y_joy, y_pos = 0;
    SDL_Rect dst_rect;
    dst_rect.x = (width - canvas->width) / 2; dst_rect.y = 0; dst_rect.w = canvas->width; dst_rect.h = height;
    SDL_Event event;
    bool quit = false;
    update_texture(width, renderer, &y_pos, &dst_rect, texture, canvas, height);
    while(!quit)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
            quit = true;
            break;
            case SDL_JOYAXISMOTION:
            y_joy = SDL_JoystickGetAxis(joystick, 1) / 4095;
            if(y_joy > 2)
            {
                y_pos += (y_joy - 2) * font_height;
            }
            else if (y_joy < -2)
            {
                y_pos += (y_joy + 2) * font_height;
            }
            update_texture(width, renderer, &y_pos, &dst_rect, texture, canvas, height);
            break;
            case SDL_KEYDOWN:
            if(event.key.keysym.mod & KMOD_LGUI || event.key.keysym.mod & KMOD_RGUI)
            {
                switch(event.key.keysym.sym)
                {
                    case 'w':
                    quit = true;
                    break;
                    default:
                    break;
                }
            }
            switch(event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                quit = true;
                break;
                case SDLK_UP:
                if(event.key.keysym.mod & KMOD_LSHIFT && event.key.keysym.mod & KMOD_RSHIFT)
                {
                    y_pos -= font_height * 4;
                }
                else if(event.key.keysym.mod & KMOD_LSHIFT || event.key.keysym.mod & KMOD_RSHIFT)
                {
                    y_pos -= font_height * 2;
                }
                else
                {
                    y_pos -= font_height / 2;
                }
                update_texture(width, renderer, &y_pos, &dst_rect, texture, canvas, height);
                break;
                case SDLK_DOWN:
                if(event.key.keysym.mod & KMOD_LSHIFT && event.key.keysym.mod & KMOD_RSHIFT)
                {
                    y_pos += font_height * 8;
                }
                else if(event.key.keysym.mod & KMOD_LSHIFT || event.key.keysym.mod & KMOD_RSHIFT)
                {
                    y_pos += font_height * 2;
                }
                else
                {
                    y_pos += font_height / 2;
                }
                update_texture(width, renderer, &y_pos, &dst_rect, texture, canvas, height);
                break;
            }
            break;
            default:
            break;
        }
    }
    SDL_DestroyTexture(texture);
}
