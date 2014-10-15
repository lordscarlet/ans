#include <stdlib.h>
#include <stdio.h>

#include "file/xbin.h"
#include "image/canvas.h"
#include "ui/window.h"

typedef enum {
    UNKNOWN, ARTWORX, ANSI, ANSIEDIT, BINARY, ICE_DRAW, PC_BOARD, TUNDRA, XBIN
} ExtensionType;

ExtensionType extension_check(char const *filename)
{
    char const *dot = strrchr(filename, '.');
    if(dot == NULL || dot == filename)
    {
        return UNKNOWN;
    }
    dot += 1;
    if(strcasecmp(dot, "adf") == 0)
    {
        return ARTWORX;
    }
    if(strcasecmp(dot, "ans") == 0)
    {
        return ANSI;
    }
    if(strcasecmp(dot, "ansiedit") == 0)
    {
        return ANSIEDIT;
    }
    if(strcasecmp(dot, "bin") == 0)
    {
        return BINARY;
    }
    if(strcasecmp(dot, "idf") == 0)
    {
        return ICE_DRAW;
    }
    if(strcasecmp(dot, "pcb") == 0)
    {
        return PC_BOARD;
    }
    if(strcasecmp(dot, "tun") == 0)
    {
        return TUNDRA;
    }
    if(strcasecmp(dot, "xb") == 0)
    {
        return XBIN;
    }
    return UNKNOWN;
}

void process_file(char const *filename, TextmodeDisplay *display)
{
    void* file;
    Canvas* canvas;
    printf("Processing file %s\n", filename);
    switch(extension_check(filename))
    {
        case UNKNOWN:
        break;
        case ARTWORX:
        break;
        case ANSI:
        break;
        case ANSIEDIT:
        break;
        case BINARY:
        break;
        case ICE_DRAW:
        break;
        case PC_BOARD:
        break;
        case TUNDRA:
        break;
        case XBIN:
        file = load_xbin(filename);
        debug_xbin_file(file);
        canvas = xbin_file_to_canvas(file);
        update_window(display, canvas);
        free_xbin_file(file);
        free_canvas(canvas);
        break;
    }
}

int main(int argc, char const *argv[])
{
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
            process_file(argv[i], display);
        }
        end_window(display);
    }
    return 0;
}
