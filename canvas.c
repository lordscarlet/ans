#include <stdlib.h>
#include "canvas.h"

Canvas* create_canvas(size_t width, size_t height)
{
    Canvas *canvas = malloc(sizeof(Canvas));
    canvas->width = width;
    canvas->height = height;
    canvas->length = width * height * 3;
    if(canvas->length > 0)
    {
        canvas->data = malloc(canvas->length);
    }
    else
    {
        canvas->data = NULL;
    }
    return canvas;
}

void free_canvas(Canvas *canvas)
{
    if(canvas != NULL)
    {
        if(canvas->data != NULL)
        {
            free(canvas->data);
        }
        free(canvas);
    }
}