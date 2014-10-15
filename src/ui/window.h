#ifndef WINDOW_H_A2935005
#define WINDOW_H_A2935005

#include <stdbool.h>
#include <SDL.h>

#include "../image/canvas.h"

typedef struct
{
    uint32_t width;
    uint32_t height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool full_screen;
} TextmodeDisplay;

TextmodeDisplay* init_window(bool full_screen);

void update_window(TextmodeDisplay *display, Canvas *canvas);

void end_window(TextmodeDisplay *display);

#endif /* end of include guard: WINDOW_H_A2935005 */
