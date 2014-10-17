#include <stdio.h>
#include <stdlib.h>

#include "tundra.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "../../image/renderer.h"
#include "palette.h"
#include "font.h"

uint32_t TUNDRA_IMAGE_CHUNK_LENGTH  = 10000;

uint32_t get_tundra_32_bit_number(FILE *file_ptr)
{
    uint8_t bytes[4];
    fread(bytes, 1, 4, file_ptr);
    return ((uint32_t) bytes[0] << 24) + ((uint32_t) bytes[1] << 16) + ((uint32_t) bytes[2] << 8) + (uint32_t) bytes[3];
}

void realloc_tundra_image_bytes_if_necessary(TundraFile *file, uint32_t *limit, uint32_t pos)
{
    void* alloc;
    uint32_t new_limit;
    if(pos >= *limit)
    {
        new_limit = *limit + TUNDRA_IMAGE_CHUNK_LENGTH * file->columns * 7;
        alloc = calloc(new_limit, 1);
        memcpy(alloc, file->image_bytes, *limit);
        free(file->image_bytes);
        file->image_bytes = alloc;
        *limit = new_limit;
    }
}

TundraFile* load_tundra(char const *filename)
{
    TundraFile *file = malloc(sizeof(TundraFile));
    FILE       *file_ptr = fopen(filename, "r");
    uint8_t    magic_number[9], ascii_code;
    uint8_t    *foreground = calloc(3, 1);
    uint8_t    *background = calloc(3, 1);
    uint32_t   image_bytes_limit;
    file->sauce = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    file->columns = t_info_1(file->sauce, 80);
    file->rows = 0;
    image_bytes_limit = TUNDRA_IMAGE_CHUNK_LENGTH * file->columns * 7;
    file->image_bytes = calloc(image_bytes_limit, 1);
    fread(magic_number, 1, 9, file_ptr);
    for(uint32_t i = 9, x = 0, y = 0, image_bytes_location; i < file->actual_file_size;)
    {
        if(x == file->columns)
        {
            x = 0;
            y += 1;
        }
        fread(&ascii_code, 1, 1, file_ptr);
        i += 1;
        if(ascii_code == 1)
        {
            y = get_tundra_32_bit_number(file_ptr);
            x = get_tundra_32_bit_number(file_ptr);
            i += 8;
        }
        if(ascii_code == 2)
        {
            fread(&ascii_code, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(foreground, 1, 3, file_ptr);
            i += 5;
        }
        if(ascii_code == 4)
        {
            fread(&ascii_code, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(background, 1, 3, file_ptr);
            i += 5;
        }
        if(ascii_code == 6)
        {
            fread(&ascii_code, 1, 1, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(foreground, 1, 3, file_ptr);
            fseek(file_ptr, 1L, SEEK_CUR);
            fread(background, 1, 3, file_ptr);
            i += 9;
        }
        if (ascii_code != 1 && ascii_code != 2 && ascii_code != 4 && ascii_code != 6) {
            image_bytes_location = (y * file->columns + x) * 7;
            realloc_tundra_image_bytes_if_necessary(file, &image_bytes_limit, image_bytes_location);
            file->image_bytes[image_bytes_location] = ascii_code;
            memcpy(file->image_bytes + image_bytes_location + 1, foreground, 3);
            memcpy(file->image_bytes + image_bytes_location + 4, background, 3);
            x += 1;
            if(y + 1 > file->rows)
            {
                file->rows = y + 1;
            }
        }
    }
    file->image_bytes = realloc(file->image_bytes, file->columns * file->rows * 7);
    fclose(file_ptr);
    free(foreground);
    free(background);
    return file;
}

void free_tundra_file(TundraFile *file)
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

void debug_tundra_file(TundraFile *file)
{
    printf("Tundra columns: %i\n", file->columns);
    printf("Tundra rows: %i\n", file->rows);
    printf("Tundra actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* tundra_file_to_canvas(TundraFile *file)
{
    Font    *font = get_preset_font(IBM_VGA_8x16);
    Canvas  *canvas = create_canvas(file->columns * font->width, file->rows * font->height);
    uint8_t ascii_code;
    for(uint32_t y = 0, i = 0; y < file->rows; y += 1)
    {
        for(uint32_t x = 0; x < file->columns; x += 1, i += 7)
        {
            ascii_code = file->image_bytes[i];
            draw_rgb_glyph(canvas, ascii_code, file->image_bytes + i + 1, file->image_bytes + i + 4, x, y, font);
        }
    }
    canvas->font_height = font->height;
    free_font(font);
    return canvas;
}

Canvas* load_tundra_file_and_generate_canvas(char const *filename)
{
    TundraFile* file = load_tundra(filename);
    debug_tundra_file(file);
    Canvas *canvas = tundra_file_to_canvas(file);
    free_tundra_file(file);
    return canvas;
}