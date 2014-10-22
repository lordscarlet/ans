#include <stdbool.h>
#include <SDL.h>
#include <time.h>

#include "event.h"
#include "canvas.h"

uint32_t MAX_TEXTURES = 8;

void draw_textures(size_t width, size_t height, SDL_Renderer *renderer, TextureCollection* textures, int32_t *x_pos, int32_t *y_pos)
{
    SDL_Rect src_rect, dst_rect;
    if(textures->height < height)
    {
        *y_pos = ((int32_t) height - (int32_t) textures->height) / 2;
    }
    else if(*y_pos < 0 || textures->canvas->height < height)
    {
        *y_pos = 0;
    }
    else if(*y_pos > textures->canvas->height - height)
    {
        *y_pos = textures->canvas->height - height;
    }
    if(textures->width < width)
    {
        *x_pos = ((int32_t) width - (int32_t) textures->width) / 2;
    }
    else if(*x_pos > 0)
    {
        *x_pos = 0;
    }
    else if(*x_pos < (int32_t) width - (int32_t) textures->width)
    {
        *x_pos = (int32_t) width - (int32_t) textures->width;
    }
    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = textures->canvas->width;
    src_rect.h = textures->max_height;
    dst_rect.x = (int) *x_pos;
    if(textures->height < height)
    {
        dst_rect.y = *y_pos;
    }
    else
    {
        dst_rect.y = (int) -(*y_pos % textures->max_height);
    }
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

void mouse_wheel_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, SDL_MouseWheelEvent *wheel, int32_t *x_pos, int32_t *y_pos)
{
    *x_pos += textures->canvas->font_height * -wheel->x;
    *y_pos += textures->canvas->font_height * -wheel->y;
    draw_textures(width, height, renderer, textures, x_pos, y_pos);
}

EventLoopReturnType mouse_button_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, SDL_MouseButtonEvent *button)
{
    if(button->state == SDL_PRESSED)
    {
        switch(button->button)
        {
            case SDL_BUTTON_LEFT:
            return EVENT_LOOP_NEXT;
            case SDL_BUTTON_RIGHT:
            return EVENT_LOOP_PREV;
            default:
            break;
        }
    }
    return EVENT_LOOP_NONE;
}

void joy_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, SDL_Joystick *joystick, int32_t *x_pos, int32_t *y_pos)
{
    int32_t x_joy, y_joy;
    SDL_Event event;
    while(true) {
        SDL_PollEvent(&event);
        x_joy = SDL_JoystickGetAxis(joystick, 0) / 4095;
        y_joy = SDL_JoystickGetAxis(joystick, 1) / 4095;
        if(x_joy > 2)
        {
            *x_pos += -x_joy - 2;
            draw_textures(width, height, renderer, textures, x_pos, y_pos);
        }
        else if (x_joy < -2)
        {
            *x_pos += -x_joy + 2;
            draw_textures(width, height, renderer, textures, x_pos, y_pos);
        }
        if(y_joy > 2)
        {
            *y_pos += y_joy - 2;
            draw_textures(width, height, renderer, textures, x_pos, y_pos);
        }
        else if (y_joy < -2)
        {
            *y_pos += y_joy + 2;
            draw_textures(width, height, renderer, textures, x_pos, y_pos);
        }
        if((x_joy >= -2 && x_joy <= 2) && (y_joy >= -2 && y_joy <= 2))
        {
            break;
        }
    }
}

EventLoopReturnType key_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, SDL_KeyboardEvent *event, int32_t *x_pos, int32_t *y_pos)
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
        draw_textures(width, height, renderer, textures, x_pos, y_pos);
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
        draw_textures(width, height, renderer, textures, x_pos, y_pos);
        break;
        case SDLK_LEFT:
        if(shift)
        {
            *x_pos += textures->canvas->font_height * 4;
        }
        else
        {
            *x_pos += textures->canvas->font_height;
        }
        draw_textures(width, height, renderer, textures, x_pos, y_pos);
        break;
        case SDLK_RIGHT:
        if(shift)
        {
            *x_pos -= textures->canvas->font_height * 4;
        }
        else
        {
            *x_pos -= textures->canvas->font_height;
        }
        draw_textures(width, height, renderer, textures, x_pos, y_pos);
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
        draw_textures(width, height, renderer, textures, x_pos, y_pos);
        break;
        case SDLK_j:
        return EVENT_LOOP_NEXT;
        case SDLK_k:
        return EVENT_LOOP_PREV;
        case SDLK_q: case SDLK_ESCAPE:
        return EVENT_LOOP_QUIT;
    }
    return EVENT_LOOP_NONE;
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

EventLoopReturnType event_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas)
{
    TextureCollection* textures = create_textures(renderer, canvas);
    SDL_Joystick *joystick = SDL_JoystickOpen(0);
    SDL_Event event;
    EventLoopReturnType event_return = EVENT_LOOP_NONE;
    int32_t x_pos = ((int32_t) width - (int32_t) canvas->width) / 2, y_pos = 0;
    generate_initial_textures(renderer, textures);
    draw_textures(width, height, renderer, textures, &x_pos, &y_pos);
    while(true)
    {
        SDL_WaitEventTimeout(&event, 50);
        switch(event.type)
        {
            case SDL_QUIT:
            event_return = EVENT_LOOP_QUIT;
            break;
            case SDL_MOUSEWHEEL:
            mouse_wheel_event(width, height, renderer, textures, &event.wheel, &x_pos, &y_pos);
            break;
            case SDL_MOUSEBUTTONDOWN:
            event_return = mouse_button_event(width, height, renderer, textures, &event.button);
            break;
            case SDL_JOYAXISMOTION:
            joy_loop(width, height, renderer, textures, joystick, &x_pos, &y_pos);
            break;
            case SDL_JOYBUTTONDOWN:
            event_return = EVENT_LOOP_QUIT;
            break;
            case SDL_KEYDOWN:
            event_return = key_event(width, height, renderer, textures, &event.key, &x_pos, &y_pos);
            break;
        }
        switch(event_return)
        {
            case EVENT_LOOP_QUIT: case EVENT_LOOP_NEXT: case EVENT_LOOP_PREV:
            free_textures(textures);
            return event_return;
            default:
            break;
        }
    }
}
