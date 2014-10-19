#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "file/file.h"
#include "image/canvas.h"
#include "ui/window.h"
#include "ui/event.h"

int main(int argc, char const *argv[])
{
    Canvas* canvas;
    EventLoopReturnType event = EVENT_LOOP_NEXT;
    bool quit = false;
    uint32_t i = 1;
    if(argc == 1)
    {
        printf("Usage: anscat [file ...]\n");
        return 0;
    }
    TextmodeDisplay *display = init_window(true);
    if(display != NULL)
    {
        while(!quit) {
            canvas = read_file_and_generate_canvas(argv[i]);
            if(canvas != NULL)
            {
                debug_canvas(canvas);
                event = update_window(display, canvas);
                free_canvas(canvas);
            }
            switch(event)
            {
                case EVENT_LOOP_QUIT:
                quit = true;
                break;
                case EVENT_LOOP_NEXT:
                i += 1;
                if(i == argc)
                {
                    quit = true;
                }
                break;
                case EVENT_LOOP_PREV:
                i -= 1;
                if(i == 0)
                {
                    quit = true;
                }
                break;
                default:
                break;
            }
        }
        end_window(display);
    }
    return 0;
}
