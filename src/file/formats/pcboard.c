#include <stdio.h>
#include <stdlib.h>

#include "pcboard.h"
#include "../sauce.h"
#include "../../image/canvas.h"
#include "screen.h"
#include "palette.h"
#include "font.h"

char     *PC_BOARD_ATTRIBUTE      = "X";
char     *PC_BOARD_CLS            = "CLS";
char     *PC_BOARD_POS            = "POS:";
uint16_t PC_BOARD_DEFAULT_COLUMNS = 80;

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

PCBoardFile* load_pcboard(char const *filename)
{
    PCBoardFile *file;
    FILE        *file_ptr;
    uint8_t     ascii_code;
    uint8_t     foreground = 7;
    uint8_t     background = 0;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint8_t     *data;
    file                   = malloc(sizeof(PCBoardFile));
    file->screen           = create_screen_with_palette_and_font(CHARACTER_AND_ATTRIBUTE_PAIR, BINARY_PALETTE, CP437_8x16);
    file_ptr               = fopen(filename, "r");
    file->sauce            = get_sauce(file_ptr);
    file->actual_file_size = get_actual_file_size(file_ptr, file->sauce);
    data = malloc(file->actual_file_size);
    fread(data, 1, file->actual_file_size, file_ptr);
    fclose(file_ptr);
    file->screen->columns = t_info_1(file->sauce, PC_BOARD_DEFAULT_COLUMNS);
    for(uint32_t i = 0; i < file->actual_file_size; i += 1)
    {
        ascii_code = data[i];
        switch(ascii_code)
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
                clear_screen(file->screen);
                i += 4;
            }
            else
            {
                goto LITERAL;
            }
            break;
            default:
LITERAL:
            put_character_and_attribute_pair_on_screen(file->screen, &x, &y, ascii_code, (background << 4) + foreground);
            // image_bytes_location = (y * file->columns + x) * 2;
            // realloc_pc_board_image_bytes_if_necessary(file, &image_bytes_limit, image_bytes_location);
            // file->image_bytes[image_bytes_location + 0] = *ascii_code;
            // file->image_bytes[image_bytes_location + 1] = (background << 4) + foreground;
            // x += 1;
            // if(y + 1 > file->rows)
            // {
            //     file->rows = y + 1;
            // }
            break;
        }
    }
    free(data);
    truncate_screen_data(file->screen);
    return file;
}

void free_pcboard_file(PCBoardFile *file)
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

void debug_pcboard_file(PCBoardFile *file)
{
    debug_screen(file->screen);
    printf("PCBoard actual file size (excluding Sauce record and comments, in bytes): %d\n", file->actual_file_size);
    if(file->sauce != NULL)
    {
        debug_sauce(file->sauce);
    }
}

Canvas* load_pcboard_file_and_generate_canvas(char const *filename)
{
    PCBoardFile* file = load_pcboard(filename);
    debug_pcboard_file(file);
    Canvas *canvas = screen_to_canvas(file->screen);
    free_pcboard_file(file);
    return canvas;
}