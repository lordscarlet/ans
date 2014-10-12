#include <sdl.h>
#include <stdbool.h>
#include "event.h"
#include "canvas.h"

bool show_window(Canvas *canvas, bool full_screen, uint8_t font_height)
{
    SDL_DisplayMode current;
    SDL_Window *window;
    SDL_Renderer *renderer;
    size_t width, height;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
    	return false;
    }
    if(full_screen)
    {
        SDL_GetCurrentDisplayMode(0, &current);
        width  = current.w;
        height = current.h;
        SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL, &window, &renderer); 
    }
    else
    {
        width  = canvas->width;
        height = 400;
        SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_OPENGL, &window, &renderer); 
    }
    if(window == NULL)
    {
        SDL_Quit();
        return false;
    }
    SDL_ShowCursor(SDL_DISABLE);
    SDL_JoystickEventState(SDL_ENABLE);
    event_loop(width, height, renderer, canvas, font_height);
    SDL_JoystickEventState(SDL_DISABLE);
    SDL_ShowCursor(SDL_ENABLE);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return true;
}
