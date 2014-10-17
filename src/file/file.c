#include "file.h"
#include "../image/canvas.h"
#include "formats/artworx.h"
#include "formats/binary.h"
#include "formats/icedraw.h"
#include "formats/pcboard.h"
#include "formats/tundra.h"
#include "formats/xbin.h"

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
    if(strcasecmp(dot, "tnd") == 0)
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
        return NULL;
        break;
        case ARTWORX:
        return load_artworx_file_and_generate_canvas(filename);
        break;
        case ANSI:
        break;
        case ANSIEDIT:
        break;
        case BINARY:
        return load_binary_file_and_generate_canvas(filename);
        break;
        case ICE_DRAW:
        return load_ice_draw_file_and_generate_canvas(filename);
        break;
        case PC_BOARD:
        return load_pcboard_file_and_generate_canvas(filename);
        break;
        case TUNDRA:
        return load_tundra_file_and_generate_canvas(filename);
        break;
        case XBIN:
        return load_xbin_file_and_generate_canvas(filename);
    }
    return NULL;
}
