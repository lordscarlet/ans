#include <stdio.h>
#include <stdlib.h>

#include "binary.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "../../image/renderer.h"
#include "font.h"

uint16_t BINARY_DEFAULT_COLUMNS = 160;
uint8_t BINARY_DEFAULT_PAL[]    = {0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x2a, 0x00, 0x00, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x2a, 0x00, 0x2a, 0x2a, 0x15, 0x00, 0x2a, 0x2a, 0x2a, 0x15, 0x15, 0x15, 0x15, 0x15, 0x3f, 0x15, 0x3f, 0x15, 0x15, 0x3f, 0x3f, 0x3f, 0x15, 0x15, 0x3f, 0x15, 0x3f, 0x3f, 0x3f, 0x15, 0x3f, 0x3f, 0x3f};

BinaryFile* load_binary(char const *filename)
{
    BinaryFile *file = malloc(sizeof(BinaryFile));
    FILE *file_ptr = fopen(filename, "r");
    file->sauce = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    file->rows = file->actual_file_size / 2 / 160;
    file->image_bytes = malloc(file->actual_file_size);
    fread(file->image_bytes, 1, file->actual_file_size, file_ptr);
    fclose(file_ptr);
    return file;
}

void free_binary_file(BinaryFile *file)
{
    if(file != NULL)
    {
        if(file->sauce != NULL)
        {
            free(file->sauce);
        }
        free(file->image_bytes);
        free(file);
    }
}

void debug_binary_file(BinaryFile *file)
{
    printf("Binary rows: %i\n", file->rows);
    printf("Binary actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* binary_file_to_canvas(BinaryFile *file)
{
    Font *font = get_preset_font(IBM_VGA_8x16);
    Canvas *canvas = create_canvas(BINARY_DEFAULT_COLUMNS * font->width, file->rows * font->height);
    uint8_t palette_rgb[48];
    uint8_t ascii_code, foreground, background;
    bool non_blink;
    if(file->sauce != NULL)
    {
        non_blink = file->sauce->non_blink;
    }
    else
    {
        non_blink = false;
    }
    convert_palette(BINARY_DEFAULT_PAL, palette_rgb);
    for(uint16_t y = 0, i = 0; y < file->rows; y += 1)
    {
        for(uint16_t x = 0; x < BINARY_DEFAULT_COLUMNS; x += 1, i += 2)
        {
            ascii_code = file->image_bytes[i];
            foreground = file->image_bytes[i + 1] & 0xf;
            background = file->image_bytes[i + 1] >> 4;
            if(!non_blink && background >= 8)
            {
                background -= 8;
            }
            draw_glyph(canvas, ascii_code, foreground, background, x, y, palette_rgb, font);
        }
    }
    canvas->font_height = 16;
    free_font(font);
    return canvas;
}

Canvas* load_binary_file_and_generate_canvas(char const *filename)
{
    BinaryFile* file = load_binary(filename);
    debug_binary_file(file);
    Canvas *canvas = binary_file_to_canvas(file);
    free_binary_file(file);
    return canvas;
}