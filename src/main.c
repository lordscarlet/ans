#include <stdlib.h>
#include <stdio.h>

#include "file/xbin.h"
#include "image/canvas.h"
#include "ui/window.h"

int main(int argc, char const *argv[])
{
    XBin_File* file = load_xbin(argv[1]);
    debug_xbin_file(file);
    Canvas* canvas = xbin_file_to_canvas(file);
    debug_canvas(canvas);
    show_window(canvas, true);
    free_xbin_file(file);
    free_canvas(canvas);
    return 0;
}
