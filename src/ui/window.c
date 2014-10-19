#include <stdbool.h>
#include <stdint.h>
#include <SDL.h>

#include "window.h"
#include "../image/canvas.h"
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
        display->height = 640;
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

EventLoopReturnType update_window(TextmodeDisplay *display, Canvas *canvas)
{
    if(!display->full_screen)
    {
        display->width = canvas->width;
        SDL_SetWindowSize(display->window, (int) display->width, (int) display->height);
    }
    return event_loop(display->width, display->height, display->renderer, canvas);
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