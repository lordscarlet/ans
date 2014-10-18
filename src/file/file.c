#include "file.h"
#include "sauce.h"
#include "formats/screen.h"
#include "../image/canvas.h"
#include "formats/artworx.h"
#include "formats/ansiedit.h"
#include "formats/ascii.h"
#include "formats/binary.h"
#include "formats/icedraw.h"
#include "formats/pcboard.h"
#include "formats/tundra.h"
#include "formats/xbin.h"

void debug_text_art_file(TextArtFile *file)
{
    debug_screen(file->screen);
    printf("Actual file size (excluding Sauce metadata): %d\n", file->length);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

void free_text_art_file(TextArtFile *file)
{
    if(file != NULL)
    {
        free_screen(file->screen);
        if(file->sauce != NULL)
        {
            free(file->sauce);
        }
        free(file);
    }
}

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
    if(strcasecmp(dot, "asc") == 0)
    {
        return ASCII;
    }
    if(strcasecmp(dot, "ansiedit") == 0)
    {
        return ANSIEDIT;
    }
    if(strcasecmp(dot, "bin") == 0)
    {
        return BINARY;
    }
    if(strcasecmp(dot, "diz") == 0)
    {
        return DIZ;
    }
    if(strcasecmp(dot, "idf") == 0)
    {
        return ICE_DRAW;
    }
    if(strcasecmp(dot, "ion") == 0)
    {
        return DIZ;
    }
    if(strcasecmp(dot, "nfo") == 0)
    {
        return ASCII;
    }
    if(strcasecmp(dot, "pcb") == 0)
    {
        return PC_BOARD;
    }
    if(strcasecmp(dot, "tnd") == 0)
    {
        return TUNDRA;
    }
    if(strcasecmp(dot, "txt") == 0)
    {
        return ASCII;
    }
    if(strcasecmp(dot, "xb") == 0)
    {
        return XBIN;
    }
    return UNKNOWN;
}

Canvas* read_file_and_generate_canvas(char const *filename)
{
    TextArtFile *file = NULL;
    Canvas *canvas    = NULL;
    uint16_t actual_columns;
    switch(extension_check(filename))
    {
        case UNKNOWN:
        return NULL;
        break;
        case ARTWORX:
        file = load_artworx_file(filename);
        break;
        case ANSI:
        break;
        case ANSIEDIT:
        file = load_ansiedit_file(filename);
        break;
        case ASCII:
        file = load_ascii_file(filename);
        break;
        case BINARY:
        file = load_binary_file(filename);
        break;
        case DIZ:
        file = load_ascii_file(filename);
        actual_columns = get_actual_columns(file->screen);
        trim_columns(file->screen, actual_columns);
        break;
        case ICE_DRAW:
        file = load_ice_draw_file(filename);
        break;
        case PC_BOARD:
        file = load_pcboard_file(filename);
        break;
        case TUNDRA:
        file = load_tundra_file(filename);
        break;
        case XBIN:
        file = load_xbin_file(filename);
        break;
    }
    if(file != NULL)
    {
        debug_text_art_file(file);
        canvas = screen_to_canvas(file->screen);
        free_text_art_file(file);
    }
    return canvas;
}

