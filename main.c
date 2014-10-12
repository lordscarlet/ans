#include <stdlib.h>
#include <stdio.h>
#include "xbin.h"
#include "window.h"
#include "canvas.h"
#include "renderer.h"

int main(int argc, char const *argv[])
{
    XBin_File* file = load_xbin(argv[1]);
    Canvas* canvas = xbin_file_to_canvas(file);
    debug_xbin_file(file);
    show_window(canvas, true, file->font_height);
    free_xbin_file(file);
    free_canvas(canvas);
    return 0;
}
