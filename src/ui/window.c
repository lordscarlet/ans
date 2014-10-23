#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

#include "window.h"
#include "../file/file.h"
#include "canvas.h"
#include "event.h"

TextmodeDisplay* init_window(bool full_screen)
{
    TextmodeDisplay *display = malloc(sizeof(TextmodeDisplay));
    SDL_DisplayMode current;
    display->full_screen = full_screen;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        return NULL;
    }
    if(display->full_screen)
    {
        SDL_GetCurrentDisplayMode(0, &current);
        display->width  = (uint32_t) current.w;
        display->height = (uint32_t) current.h;
        SDL_CreateWindowAndRenderer(display->width, display->height, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL, &display->window, &display->renderer);
        SDL_ShowCursor(SDL_DISABLE);
    }
    else
    {
        display->width  = 640;
        display->height = 400;
        SDL_CreateWindowAndRenderer(display->width, display->height, SDL_WINDOW_OPENGL, &display->window, &display->renderer);
    }
    if(display->window == NULL)
    {
        SDL_Quit();
        return NULL;
    }
    SDL_JoystickEventState(SDL_ENABLE);
    return display;
}

void end_window(TextmodeDisplay *display)
{
    SDL_JoystickEventState(SDL_DISABLE);
    if(display->full_screen)
    {
        SDL_ShowCursor(SDL_ENABLE);
    }
    SDL_DestroyRenderer(display->renderer);
    SDL_DestroyWindow(display->window);
    SDL_Quit();
    free(display);
}

void display_window(char **filenames, uint32_t filenames_length, bool display_full_screen)
{
    TextArtFile *file;
    Canvas *canvas;
    EventLoopReturnType event = EVENT_LOOP_NEXT;
    bool quit = false;
    int64_t i = 0;
    uint16_t current_file_index;
    TextmodeDisplay *display = init_window(display_full_screen);
    ViewPrefs view_prefs;
    view_prefs.file_list = false;
    view_prefs.sauce_info = false;
    view_prefs.title = false;
    if(display != NULL)
    {
        while(!quit) {
            file = read_text_art_file(filenames[i]);
            if(file != NULL)
            {
                canvas = screen_to_canvas(file->screen);
                canvas->file = file;
                if(!display->full_screen)
                {
                    display->width = canvas->width;
                    SDL_SetWindowSize(display->window, (int) display->width, (int) display->height);
                }
                current_file_index = (uint16_t) i;
                event = event_loop(display->width, display->height, display->renderer, canvas, filenames, filenames_length, &current_file_index, &view_prefs);
                free_canvas(canvas);
            }
            switch(event)
            {
                case EVENT_LOOP_QUIT:
                quit = true;
                break;
                case EVENT_LOOP_NEXT:
                i += 1;
                if(i == filenames_length)
                {
                    quit = true;
                }
                break;
                case EVENT_LOOP_PREV:
                i -= 1;
                if(i == -1)
                {
                    quit = true;
                }
                break;
                default:
                break;
            }
        }
        end_window(display);
    }
}