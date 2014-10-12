#ifndef EVENT_H_B8F487E5
#define EVENT_H_B8F487E5

#include <sdl.h>
#include "canvas.h"

void event_loop(size_t width, size_t height, SDL_Renderer *renderer, Canvas *canvas, uint8_t font_height);

#endif /* end of include guard: EVENT_H_B8F487E5 */
