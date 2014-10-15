#include "file.h"
#include "../image/canvas.h"
#include "xbin.h"

FileType extension_check(char const *filename)
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

Canvas* read_file_and_generate_canvas(char const *filename)
{
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
        return load_xbin_file_and_generate_canvas(filename);
    }
    return NULL;
}
