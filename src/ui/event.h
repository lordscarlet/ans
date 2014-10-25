#ifndef EVENT_H_B8F487E5
#define EVENT_H_B8F487E5

#include <stdint.h>
#include <SDL.h>
#include "canvas.h"
#include "window.h"

typedef enum {
    EVENT_LOOP_NONE,
    EVENT_LOOP_NEXT,
    EVENT_LOOP_PREV,
    EVENT_LOOP_QUIT,
    EVENT_LOOP_LETTER_SPACING,
    EVENT_LOOP_TITLE,
    EVENT_LOOP_FILE_LIST,
    EVENT_LOOP_INFO,
    EVENT_LOOP_GLYPH,
    EVENT_LOOP_SAUCE
} EventLoopReturnType;

EventLoopReturnType event_loop(uint32_t width, uint32_t height, SDL_Renderer *renderer, Canvas *canvas, char **filenames, uint32_t filenames_length, uint16_t *current_filename_index, ViewPrefs *view_prefs);

#endif /* end of include guard: EVENT_H_B8F487E5 */
