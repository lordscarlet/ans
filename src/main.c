#include <stdlib.h>
#include <stdio.h>

#include "file/file.h"
#include "image/canvas.h"
#include "ui/window.h"

int main(int argc, char const *argv[])
{
    Canvas* canvas;
    if(argc == 1)
    {
        printf("Usage: anscat [file ...]\n");
        return 0;
    }
    TextmodeDisplay *display = init_window(true);
    if(display != NULL)
    {
        for(size_t i = 1; i < argc; i += 1)
        {
            canvas = read_file_and_generate_canvas(argv[i]);
            if(canvas != NULL)
            {
                debug_canvas(canvas);
                update_window(display, canvas);
                free_canvas(canvas);
            }
        }
        end_window(display);
    }
    return 0;
}
