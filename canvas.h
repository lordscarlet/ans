#ifndef CANVAS_H_FC7B83B1
#define CANVAS_H_FC7B83B1

#include <stdint.h>

typedef struct
{
    size_t width;
    size_t height;
    uint8_t *data;
    uint32_t length;
} Canvas;

Canvas* create_canvas(size_t width, size_t height);

void free_canvas(Canvas *canvas);

#endif /* end of include guard: CANVAS_H_FC7B83B1 */
