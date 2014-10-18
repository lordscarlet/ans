#include <stdio.h>
#include <stdlib.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

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

TextArtFile* load_pcboard_file(char const *filename)
{
    TextArtFile *file;
    FILE        *file_ptr;
    uint8_t     ascii_code;
    uint8_t     foreground = 7;
    uint8_t     background = 0;
    uint16_t    x = 0;
    uint16_t    y = 0;
    uint8_t     *data;
    file                    = malloc(sizeof(TextArtFile));
    file->screen            = create_screen_with_palette_and_font(CHARACTER_AND_ATTRIBUTE_PAIR, BINARY_PALETTE, CP437_8x16);
    file->screen->non_blink = true;
    file_ptr                = fopen(filename, "r");
    file->sauce             = get_sauce(file_ptr);
    file->length            = get_real_file_size(file_ptr, file->sauce);
    data = malloc(file->length);
    fread(data, 1, file->length, file_ptr);
    fclose(file_ptr);
    file->screen->columns = t_info_1(file->sauce, PC_BOARD_DEFAULT_COLUMNS);
    for(uint32_t i = 0; i < file->length; i += 1)
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
            if(look_ahead(data, i, file->length, PC_BOARD_ATTRIBUTE))
            {
                if(i + 3 < file->length)
                {
                    background = get_color(data[i + 2]);
                    foreground = get_color(data[i + 3]);
                    i += 3;
                }
            }
            else if(look_ahead(data, i, file->length, PC_BOARD_POS))
            {
                if(i + 6 < file->length)
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
            else if(look_ahead(data, i, file->length, PC_BOARD_CLS))
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
            break;
        }
    }
    free(data);
    truncate_screen_data(file->screen);
    return file;
}
