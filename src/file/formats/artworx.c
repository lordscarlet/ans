#include <stdio.h>
#include <stdlib.h>

#include "artworx.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "../../image/renderer.h"
#include "palette.h"
#include "font.h"

uint8_t ADF_PALETTE_ORDERING[] = {0, 1, 2, 3, 4, 5, 20, 7, 56, 57, 58, 59, 60, 61, 62, 63};
size_t  ADF_VERSION_LENGTH     = 1;
size_t  ADF_PALETTE_LENGTH     = 192;
size_t  ADF_FONT_LENGTH        = 4096;

ArtworxFile* load_artworx(char const *filename)
{
    uint8_t version_number;
    uint32_t image_bytes_length;
    FILE *file_ptr = fopen(filename, "r");
    ArtworxFile *file = malloc(sizeof(ArtworxFile));
    uint8_t full_palette[ADF_PALETTE_LENGTH];
    file->sauce = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    fread(&version_number, 1, ADF_VERSION_LENGTH, file_ptr);
    fread(full_palette, 1, ADF_PALETTE_LENGTH, file_ptr);
    file->palette = create_new_palette();
    for(uint8_t i = 0; i < 16; i += 1)
    {
        for(uint8_t j = 0, from = ADF_PALETTE_ORDERING[i] * 3, to = i * 3; j < 3; j += 1, from += 1, to += 1)
        {
            file->palette->bytes[to] = full_palette[from];
        }
    }
    generate_rgb_data(file->palette);
    file->font = load_8x16x256_font(file_ptr);
    image_bytes_length = file->actual_file_size - (ADF_VERSION_LENGTH + ADF_PALETTE_LENGTH + ADF_FONT_LENGTH);
    file->rows = (uint16_t) (image_bytes_length / 2 / 80);
    file->image_bytes = malloc((size_t) image_bytes_length);
    fread(file->image_bytes, 1, (size_t) image_bytes_length, file_ptr);
    fclose(file_ptr);
    return file;
}

void free_artworx_file(ArtworxFile *file)
{
    if(file != NULL)
    {
        free(file->image_bytes);
        if(file->sauce != NULL)
        {
            free(file->sauce);
        }
        free_palette(file->palette);
        free_font(file->font);
        free(file);
    }
}

void debug_artworx_file(ArtworxFile *file)
{
    printf("Artworx rows: %i\n", file->rows);
    printf("Artworx palette: ");
    for(size_t i = 0; i < 48; i += 3) {
        printf("(%d, %d, %d)", file->palette->bytes[i], file->palette->bytes[i + 1], file->palette->bytes[i + 2]);
        if(i < 45)
        {
            printf(", ");
        }
    }
    printf("\n");
    printf("Artworx actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* artworx_file_to_canvas(ArtworxFile *file)
{
    Canvas *canvas = create_canvas(640, file->rows * 16);
    uint8_t ascii_code, foreground, background;
    for(uint32_t y = 0, i = 0; y < file->rows; y += 1)
    {
        for(uint32_t x = 0; x < 80; x += 1, i += 2)
        {
            ascii_code = file->image_bytes[i];
            foreground = file->image_bytes[i + 1] & 0xf;
            background = file->image_bytes[i + 1] >> 4;
            draw_glyph(canvas, ascii_code, foreground, background, x, y, file->palette, file->font);
        }
    }
    canvas->font_height = file->font->height;
    return canvas;
}

Canvas* load_artworx_file_and_generate_canvas(char const *filename)
{
    ArtworxFile* file = load_artworx(filename);
    debug_artworx_file(file);
    Canvas *canvas = artworx_file_to_canvas(file);
    free_artworx_file(file);
    return canvas;
}