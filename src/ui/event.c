#include <stdbool.h>
#include <SDL.h>
#include <time.h>

#include "event.h"
#include "canvas.h"
#include "window.h"

uint32_t MAX_TEXTURES = 8;

typedef struct {
    SDL_Rect    src_rect, dst_rect;
    uint32_t    width, height;
    bool        visible;
    bool        force_visibility;
    bool        vanishes;
    size_t      delay;
    int         start;
    SDL_Texture *texture;
} Overlay;

typedef struct {
    Overlay **data;
    size_t  length;
} OverlayCollection;

void draw_textures(size_t width, size_t height, SDL_Renderer *renderer, TextureCollection* textures, OverlayCollection *overlays, int32_t *x_pos, int32_t *y_pos)
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
    for(size_t i = 0; i < overlays->length; i += 1)
    {
        if(overlays->data[i] != NULL)
        {
            if(overlays->data[i]->force_visibility)
            {
                SDL_RenderCopy(renderer, overlays->data[i]->texture, &overlays->data[i]->src_rect, &overlays->data[i]->dst_rect);
            }
            else
            if(overlays->data[i]->visible)
            {
                if(overlays->data[i]->vanishes)
                {
                    if(overlays->data[i]->start == -1)
                    {
                        overlays->data[i]->start = clock();
                    }
                    else if(((clock() - overlays->data[i]->start) * 1000) / CLOCKS_PER_SEC > overlays->data[i]->delay)
                    {
                        overlays->data[i]->start = -1;
                        overlays->data[i]->visible = false; 
                    }
                }
                if(overlays->data[i]->visible)
                {
                    SDL_RenderCopy(renderer, overlays->data[i]->texture, &overlays->data[i]->src_rect, &overlays->data[i]->dst_rect);
                }
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void mouse_wheel_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection *textures, OverlayCollection *overlays, SDL_MouseWheelEvent *wheel, int32_t *x_pos, int32_t *y_pos)
{
    *x_pos += textures->canvas->file->screen->font->width * -wheel->x;
    *y_pos += textures->canvas->file->screen->font->height * -wheel->y;
    draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
}

EventLoopReturnType mouse_button_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection *textures, OverlayCollection *overlays, SDL_MouseButtonEvent *button)
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

void joy_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, OverlayCollection *overlays, SDL_Joystick *joystick, int32_t *x_pos, int32_t *y_pos)
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
            draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        }
        else if (x_joy < -2)
        {
            *x_pos += -x_joy + 2;
            draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        }
        if(y_joy > 2)
        {
            *y_pos += y_joy - 2;
            draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        }
        else if (y_joy < -2)
        {
            *y_pos += y_joy + 2;
            draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        }
        if((x_joy >= -2 && x_joy <= 2) && (y_joy >= -2 && y_joy <= 2))
        {
            break;
        }
    }
}

EventLoopReturnType key_event(uint32_t width, uint32_t height, SDL_Renderer *renderer, TextureCollection* textures, OverlayCollection *overlays, SDL_KeyboardEvent *event, int32_t *x_pos, int32_t *y_pos)
{
    
    bool shift = (event->keysym.mod & KMOD_LSHIFT) || (event->keysym.mod & KMOD_RSHIFT);
    switch(event->keysym.sym)
    {
        case SDLK_DOWN:
        if(shift)
        {
            *y_pos += textures->canvas->file->screen->font->height * 4;
        }
        else
        {
            *y_pos += textures->canvas->file->screen->font->height;
        }
        draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        break;
        case SDLK_UP:
        if(shift)
        {
            *y_pos -= textures->canvas->file->screen->font->height * 4;
        }
        else
        {
            *y_pos -= textures->canvas->file->screen->font->height;
        }
        draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        break;
        case SDLK_LEFT:
        if(shift)
        {
            *x_pos += textures->canvas->file->screen->font->width * 4;
        }
        else
        {
            *x_pos += textures->canvas->file->screen->font->width;
        }
        draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        break;
        case SDLK_RIGHT:
        if(shift)
        {
            *x_pos -= textures->canvas->file->screen->font->width * 4;
        }
        else
        {
            *x_pos -= textures->canvas->file->screen->font->width;
        }
        draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
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
        draw_textures(width, height, renderer, textures, overlays, x_pos, y_pos);
        break;
        case SDLK_j:
        return EVENT_LOOP_NEXT;
        case SDLK_k:
        return EVENT_LOOP_PREV;
        case SDLK_q: case SDLK_ESCAPE:
        return EVENT_LOOP_QUIT;
        case SDLK_t:
        return EVENT_LOOP_TITLE;
        case SDLK_l:
        return EVENT_LOOP_FILE_LIST;
        case SDLK_s:
        return EVENT_LOOP_SAUCE;
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

Overlay *create_overlay(SDL_Texture *texture)
{
    int width, height;
    Overlay *overlay = malloc(sizeof(Overlay));
    overlay->texture = texture;
    SDL_QueryTexture(overlay->texture, NULL, NULL, &width, &height);
    overlay->width = (uint32_t) width;
    overlay->height = (uint32_t) height;
    overlay->src_rect.x = 0;
    overlay->src_rect.y = 0;
    overlay->src_rect.w = (int) overlay->width;
    overlay->src_rect.h = (int) overlay->height;
    overlay->dst_rect.w = (int) overlay->width;
    overlay->dst_rect.h = (int) overlay->height;
    overlay->visible = false;
    overlay->force_visibility = false;
    overlay->vanishes = false;
    overlay->delay = 0;
    overlay->start = -1;
    return overlay;
}

Overlay *create_title_overlay(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas, Palette *palette, Font *font)
{
    SDL_Texture *texture = create_title_texture(renderer, canvas->file->name, canvas->file->sauce, palette, font);
    Overlay *overlay;
    if(texture == NULL)
    {
        return NULL;
    }
    overlay = create_overlay(texture);
    overlay->dst_rect.x = (width - overlay->width) / 2;
    overlay->dst_rect.y = height - 64;
    overlay->visible = true;
    overlay->vanishes = true;
    overlay->delay = 150;
    return overlay;
}

Overlay *create_sauce_overlay(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas, Palette *palette, Font *font)
{
    SDL_Texture *texture = create_sauce_texture(renderer, canvas->file->sauce, palette, font);
    Overlay *overlay = create_overlay(texture);
    overlay->dst_rect.x = width - overlay->width - 16;
    overlay->dst_rect.y = 16;
    return overlay;
}

Overlay *create_filename_list_overlay(uint32_t height, SDL_Renderer *renderer, Canvas *canvas, char **filenames, uint32_t filenames_length, uint16_t current_filename_index, Palette *palette, Font *font)
{
    SDL_Texture *texture = create_filename_list_texture(height, renderer, filenames, filenames_length, current_filename_index, palette, font);
    Overlay *overlay = create_overlay(texture);
    overlay->dst_rect.x = 16;
    overlay->dst_rect.y = 16;
    if(filenames_length > 0)
    {
        overlay->visible = true;
    }
    else
    {
        overlay->visible = false;
    }
    overlay->vanishes = true;
    overlay->delay = 100;
    return overlay;
}

OverlayCollection *create_overlays(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas, char **filenames, uint32_t filenames_length, uint16_t current_filename_index, ViewPrefs *view_prefs)
{
    Palette *palette = get_preset_palette(ANSI_PALETTE);
    Font *font = get_preset_font(CP437_8x16);
    OverlayCollection *overlay_collection = malloc(sizeof(OverlayCollection));
    overlay_collection->length = 3;
    overlay_collection->data = malloc(sizeof(Overlay*) * overlay_collection->length);
    overlay_collection->data[0] = create_title_overlay(width, height, renderer, canvas, palette, font);
    if(overlay_collection->data[0] != NULL)
    {
        if(view_prefs->title)
        {
            overlay_collection->data[0]->force_visibility = true;
        }
    }
    overlay_collection->data[1] = create_sauce_overlay(width, height, renderer, canvas, palette, font);
    if(view_prefs->sauce_info)
    {
        overlay_collection->data[1]->visible = true;
    }
    overlay_collection->data[2] = create_filename_list_overlay(height, renderer, canvas, filenames, filenames_length, current_filename_index, palette, font);
    if(view_prefs->file_list)
    {
        overlay_collection->data[2]->force_visibility = true;
    }
    free_palette(palette);
    free_font(font);
    return overlay_collection;
}

void free_overlays(OverlayCollection *overlays)
{
    for(size_t i = 0; i < overlays->length; i += 1)
    {
        if(overlays->data[i] != NULL)
        {
            SDL_DestroyTexture(overlays->data[i]->texture);
            free(overlays->data[i]);
        }
    }
    free(overlays->data);
    free(overlays);
}

EventLoopReturnType event_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas, char **filenames, uint32_t filenames_length, uint16_t *current_filename_index, ViewPrefs *view_prefs)
{
    TextureCollection *textures = create_textures(renderer, canvas);
    OverlayCollection *overlays = create_overlays(width, height, renderer, canvas, filenames, filenames_length, *current_filename_index, view_prefs);
    SDL_Joystick *joystick = SDL_JoystickOpen(0);
    SDL_Event event;
    EventLoopReturnType event_return;
    int32_t x_pos = ((int32_t) width - (int32_t) canvas->width) / 2, y_pos = 0;
    generate_initial_textures(renderer, textures);
    draw_textures(width, height, renderer, textures, overlays, &x_pos, &y_pos);
    while(true)
    {
        event_return = EVENT_LOOP_NONE;
        SDL_WaitEventTimeout(&event, 50);
        switch(event.type)
        {
            case SDL_QUIT:
            event_return = EVENT_LOOP_QUIT;
            break;
            case SDL_MOUSEWHEEL:
            mouse_wheel_event(width, height, renderer, textures, overlays, &event.wheel, &x_pos, &y_pos);
            break;
            case SDL_MOUSEBUTTONDOWN:
            event_return = mouse_button_event(width, height, renderer, textures, overlays, &event.button);
            break;
            case SDL_JOYAXISMOTION:
            joy_loop(width, height, renderer, textures, overlays, joystick, &x_pos, &y_pos);
            break;
            case SDL_JOYBUTTONDOWN:
            event_return = EVENT_LOOP_QUIT;
            break;
            case SDL_KEYDOWN:
            event_return = key_event(width, height, renderer, textures, overlays, &event.key, &x_pos, &y_pos);
            break;
        }
        switch(event_return)
        {
            case EVENT_LOOP_QUIT: case EVENT_LOOP_NEXT: case EVENT_LOOP_PREV:
            SDL_JoystickClose(joystick);
            free_overlays(overlays);
            free_textures(textures);
            return event_return;
            break;
            case EVENT_LOOP_TITLE:
            if(overlays->data[0] != NULL)
            {
                view_prefs->title = !overlays->data[0]->visible;
                overlays->data[0]->start = -2;
                overlays->data[0]->force_visibility = view_prefs->title;
                overlays->data[0]->visible = view_prefs->title;
                draw_textures(width, height, renderer, textures, overlays, &x_pos, &y_pos);
            }
            break;
            case EVENT_LOOP_FILE_LIST:
            view_prefs->file_list = !overlays->data[2]->visible;
            overlays->data[2]->start = -2;
            overlays->data[2]->force_visibility = view_prefs->file_list;
            overlays->data[2]->visible = view_prefs->file_list;
            draw_textures(width, height, renderer, textures, overlays, &x_pos, &y_pos);
            break;
            case EVENT_LOOP_SAUCE:
            view_prefs->sauce_info = !overlays->data[1]->visible;
            overlays->data[1]->start = -2;
            overlays->data[1]->force_visibility = view_prefs->sauce_info;
            overlays->data[1]->visible = view_prefs->sauce_info;
            draw_textures(width, height, renderer, textures, overlays, &x_pos, &y_pos);
            break;
            default:
            break;
        }
        for(size_t i = 0; i < overlays->length; i += 1)
        {
            if(overlays->data[i] != NULL)
            {
                if(overlays->data[i]->visible)
                {
                    if(overlays->data[i]->start == -1)
                    {
                        draw_textures(width, height, renderer, textures, overlays, &x_pos, &y_pos);
                    }
                    else if(overlays->data[i]->start != -2 && overlays->data[i]->vanishes && (((clock() - overlays->data[i]->start) * 1000) / CLOCKS_PER_SEC > overlays->data[i]->delay))
                    {
                        draw_textures(width, height, renderer, textures, overlays, &x_pos, &y_pos);
                    }
                }
            }
        }
    }
}
