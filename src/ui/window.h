#ifndef WINDOW_H_A2935005
#define WINDOW_H_A2935005

#include <stdbool.h>
#include <SDL.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool full_screen;
} TextmodeDisplay;

typedef struct
{
    bool file_list;
    bool sauce_info;
    bool title;
} ViewPrefs;

void display_window(char **filnames, uint32_t filenames_length, bool display_full_screen);

#endif /* end of include guard: WINDOW_H_A2935005 */
