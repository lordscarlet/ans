#include <stdio.h>
#include <stdlib.h>

#include "icedraw.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "../../image/renderer.h"
#include "palette.h"
#include "font.h"

uint32_t ICE_DRAW_FONT_SIZE           = 4096;
uint32_t ICE_DRAW_PALETTE_SIZE        = 48;
uint32_t ICE_DRAW_IMAGE_DATA_START    = 12;
uint8_t  ICE_DRAW_DEFAULT_FONT_HEIGHT = 16;
uint32_t ICE_DRAW_IMAGE_CHUNK_LENGTH  = 10000 * 80 * 2;

void* realloc_ice_draw_image_bytes_if_necessary(uint8_t *image_bytes, uint32_t *limit, uint32_t current_size)
{
    void* alloc = image_bytes;
    if(current_size == *limit)
    {
        *limit += ICE_DRAW_IMAGE_CHUNK_LENGTH;
        alloc = realloc(image_bytes, *limit);
    }
    return alloc;
}

IceDrawFile* load_ice_draw(char const *filename)
{
    IceDrawFile *file = malloc(sizeof(IceDrawFile));
    FILE        *file_ptr = fopen(filename, "r");
    uint32_t    image_data_end;
    uint32_t    image_bytes_limit = ICE_DRAW_IMAGE_CHUNK_LENGTH;
    uint16_t    c, uncompressed_data_size = 0;
    uint8_t     loop, ascii_code, attribute;
    file->sauce = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    image_data_end = file->actual_file_size - (ICE_DRAW_FONT_SIZE + ICE_DRAW_PALETTE_SIZE);
    fseek(file_ptr, (long) ICE_DRAW_IMAGE_DATA_START, SEEK_SET);
    file->image_bytes = malloc(image_bytes_limit);
    for(uint32_t i = ICE_DRAW_IMAGE_DATA_START; i < image_data_end;)
    {
        fread(&ascii_code, 1, 1, file_ptr);
        fread(&attribute,  1, 1, file_ptr);
        i += 2;
        c = ((uint16_t) attribute << 8) + ascii_code;
        if(c == 1)
        {
            fread(&loop, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(&ascii_code, 1, 1, file_ptr);
            fread(&attribute,  1, 1, file_ptr);
            i += 4;
            for(size_t k = 0; k < loop; k += 1, uncompressed_data_size += 2)
            {
                file->image_bytes = realloc_ice_draw_image_bytes_if_necessary(file->image_bytes, &image_bytes_limit, uncompressed_data_size);
                file->image_bytes[uncompressed_data_size + 0] = ascii_code;
                file->image_bytes[uncompressed_data_size + 1] = attribute;
            }
        }
        else
        {
            file->image_bytes = realloc_ice_draw_image_bytes_if_necessary(file->image_bytes, &image_bytes_limit, uncompressed_data_size);
            file->image_bytes[uncompressed_data_size + 0] = ascii_code;
            file->image_bytes[uncompressed_data_size + 1] = attribute;
            uncompressed_data_size += 2;
        }
    }
    file->rows = uncompressed_data_size / 2 / 80;
    file->image_bytes = realloc(file->image_bytes, file->rows * 80 * 2);
    fseek(file_ptr, (long) file->actual_file_size - (ICE_DRAW_FONT_SIZE + ICE_DRAW_PALETTE_SIZE), SEEK_SET);
    file->font    = load_8x16x256_font(file_ptr);
    file->palette = load_palette(file_ptr);
    fclose(file_ptr);
    return file;
}

void free_ice_draw_file(IceDrawFile *file)
{
    if(file != NULL)
    {
        if(file->sauce != NULL)
        {
            free(file->sauce);
        }
        free(file->image_bytes);
        free_palette(file->palette);
        free_font(file->font);
        free(file);
    }
}

void debug_ice_draw_file(IceDrawFile *file)
{
    debug_palette(file->palette);
    printf("Ice Draw actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* ice_draw_file_to_canvas(IceDrawFile *file)
{
    Canvas *canvas = create_canvas(80 * file->font->width, file->rows * file->font->height);
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

Canvas* load_ice_draw_file_and_generate_canvas(char const *filename)
{
    IceDrawFile* file = load_ice_draw(filename);
    debug_ice_draw_file(file);
    Canvas *canvas = ice_draw_file_to_canvas(file);
    free_ice_draw_file(file);
    return canvas;
}