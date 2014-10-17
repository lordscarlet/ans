#include <stdio.h>
#include <stdlib.h>

#include "pcboard.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "../../image/renderer.h"
#include "font.h"

char     *PC_BOARD_ATTRIBUTE         = "X";
char     *PC_BOARD_CLS               = "CLS";
char     *PC_BOARD_POS               = "POS:";
uint32_t PCBOARD_IMAGE_CHUNK_LENGTH  = 10000;
uint8_t  PCBOARD_DEFAULT_PAL[]       = {0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x2a, 0x00, 0x00, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x2a, 0x00, 0x2a, 0x2a, 0x15, 0x00, 0x2a, 0x2a, 0x2a, 0x15, 0x15, 0x15, 0x15, 0x15, 0x3f, 0x15, 0x3f, 0x15, 0x15, 0x3f, 0x3f, 0x3f, 0x15, 0x15, 0x3f, 0x15, 0x3f, 0x3f, 0x3f, 0x15, 0x3f, 0x3f, 0x3f};

bool look_ahead(uint8_t *data, uint32_t pos, uint32_t data_length, char *string)
{
    uint32_t string_length = (uint32_t) strlen(string);
    if(pos + string_length + 1 >= data_length)
    {
        return false;
    }
    for(uint32_t i = 0; i < string_length; i += 1)
    {
        if(data[pos + i + 1] != string[i])
        {
            return false;
        }
    }
    return true;
}

uint8_t get_color(uint8_t code)
{
    if(code >= 65)
    {
        code -= 55;
    }
    return (code - 48) & 0xf;
}

void realloc_pc_board_image_bytes_if_necessary(PCBoardFile *file, uint32_t *limit, uint32_t pos)
{
    void* alloc;
    uint32_t new_limit;
    if(pos >= *limit)
    {
        new_limit = *limit + PCBOARD_IMAGE_CHUNK_LENGTH * file->columns * 2;
        alloc = calloc(new_limit, 1);
        memcpy(alloc, file->image_bytes, *limit);
        free(file->image_bytes);
        file->image_bytes = alloc;
        *limit = new_limit;
    }
}

PCBoardFile* load_pcboard(char const *filename)
{
    PCBoardFile *file = malloc(sizeof(PCBoardFile));
    FILE     *file_ptr = fopen(filename, "r");
    uint8_t  *ascii_code, foreground = 7, background = 0, x = 0, y = 0;
    uint32_t image_bytes_limit;
    file->sauce = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    uint8_t  data[file->actual_file_size];
    file->columns = t_info_1(file->sauce, 80);
    image_bytes_limit = PCBOARD_IMAGE_CHUNK_LENGTH * file->columns * 2;
    file->rows = 0;
    file->image_bytes = calloc(image_bytes_limit, 1);
    fread(data, 1, file->actual_file_size, file_ptr);
    fclose(file_ptr);
    for(uint32_t i = 0, image_bytes_location; i < file->actual_file_size; i += 1)
    {
        ascii_code = data + i;
        switch(*ascii_code)
        {
            case '\t':
            x += 9;
            if(x > 79)
            {
                x = 79;
            }
            break;
            case '\n':
            x = 0;
            y += 1;
            break;
            case '\r':
            case 26:
            break;
            case '@':
            if(look_ahead(data, i, file->actual_file_size, PC_BOARD_ATTRIBUTE))
            {
                if(i + 3 < file->actual_file_size)
                {
                    background = get_color(data[i + 2]);
                    foreground = get_color(data[i + 3]);
                    i += 3;
                }
            }
            else if(look_ahead(data, i, file->actual_file_size, PC_BOARD_POS))
            {
                if(i + 6 < file->actual_file_size)
                {
                    if(data[i + 6] == '@')
                    {
                        x = data[i + 5] - 48 - 1;
                        i += 5;
                    }
                    else
                    {
                        x = 10 * (data[i + 5] - 48 + data[i + 6] - 48) - 1;
                        i += 6;
                    }
                }
            }
            else if(look_ahead(data, i, file->actual_file_size, PC_BOARD_CLS))
            {
                x = 0;
                y = 0;
                file->rows = 0;
                i += 4;
            }
            else
            {
                goto LITERAL;
            }
            break;
            default:
LITERAL:
            if(x == file->columns)
            {
                x = 0;
                y += 1;
            }
            image_bytes_location = (y * file->columns + x) * 2;
            realloc_pc_board_image_bytes_if_necessary(file, &image_bytes_limit, image_bytes_location);
            file->image_bytes[image_bytes_location + 0] = *ascii_code;
            file->image_bytes[image_bytes_location + 1] = (background << 4) + foreground;
            x += 1;
            if(y + 1 > file->rows)
            {
                file->rows = y + 1;
            }
            break;
        }
    }
    file->image_bytes = realloc(file->image_bytes, file->columns * file->rows * 2);
    return file;
}

void free_pcboard_file(PCBoardFile *file)
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

void debug_pcboard_file(PCBoardFile *file)
{
    printf("PCBoard columns: %i\n", file->columns);
    printf("PCBoard rows: %i\n", file->rows);
    printf("PCBoard actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* pcboard_file_to_canvas(PCBoardFile *file)
{
    Font *font = get_preset_font(IBM_VGA_8x16);
    Canvas *canvas = create_canvas(file->columns * font->width, file->rows * font->height);
    uint8_t palette_rgb[48];
    uint8_t ascii_code, foreground, background;
    convert_palette(PCBOARD_DEFAULT_PAL, palette_rgb);
    for(uint32_t y = 0, i = 0; y < file->rows; y += 1)
    {
        for(uint32_t x = 0; x < file->columns; x += 1, i += 2)
        {
            ascii_code = file->image_bytes[i];
            foreground = file->image_bytes[i + 1] & 0xf;
            background = file->image_bytes[i + 1] >> 4;
            draw_glyph(canvas, ascii_code, foreground, background, x, y, palette_rgb, font);
        }
    }
    canvas->font_height = font->height;
    free_font(font);
    return canvas;
}

Canvas* load_pcboard_file_and_generate_canvas(char const *filename)
{
    PCBoardFile* file = load_pcboard(filename);
    debug_pcboard_file(file);
    Canvas *canvas = pcboard_file_to_canvas(file);
    free_pcboard_file(file);
    return canvas;
}