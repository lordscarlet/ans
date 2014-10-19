#ifndef EVENT_H_B8F487E5
#define EVENT_H_B8F487E5

#include <stdint.h>
#include <SDL.h>
#include "../image/canvas.h"

typedef enum {
    EVENT_LOOP_NONE,
    EVENT_LOOP_NEXT,
    EVENT_LOOP_PREV,
    EVENT_LOOP_QUIT
} EventLoopReturnType;

EventLoopReturnType event_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas);

#endif /* end of include guard: EVENT_H_B8F487E5 */
