#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint16_t ANSI_DEFAULT_COLUMNS = 80;

typedef struct {
    uint8_t  *values;
    uint32_t pos, length, max_length;
} EscapeCodes;

void start_new_escape_sequence(EscapeCodes *codes)
{
    codes->pos      = 0;
    codes->length   = 0;
    codes->values[0] = 0;
}

EscapeCodes* create_escape_codes()
{
    EscapeCodes* codes = malloc(sizeof(EscapeCodes));
    codes->max_length  = 256;
    codes->values       = malloc(sizeof(uint8_t) * codes->max_length);
    start_new_escape_sequence(codes);
    return codes;
}

void free_escape_codes(EscapeCodes *codes)
{
    if(codes != NULL)
    {
        free(codes->values);
        free(codes);
    }
}

void add_numeric_to_escape_codes(EscapeCodes *codes, uint8_t ascii_code)
{
    codes->values[codes->pos] = codes->values[codes->pos] * 10 + (ascii_code - '0');
    codes->length = codes->pos + 1;
}

void append_escape_code(EscapeCodes *codes)
{
    if(codes->length == codes->max_length)
    {
        codes->max_length += 256;
        codes->values = realloc(codes->values, sizeof(uint8_t) * codes->max_length);
    }
    codes->pos += 1;
    codes->values[codes->pos] = 0;
}

void check_limits(int32_t *x, int32_t *y, Screen *screen)
{
    if(*x >= screen->columns)
    {
        *x = screen->columns - 1;
    }
    else if(*x < 0)
    {
        *x = 0;
    }
}

TextArtFile* load_ansi_file(char *filename)
{
    TextArtFile *file;
    FILE        *file_ptr;
    uint8_t     ascii_code;
    uint8_t     foreground = 7;
    uint8_t     background = 0;
    bool        foreground_rgb_mode = false;
    bool        background_rgb_mode = false;
    bool        uses_rgb_colors     = false;
    uint8_t     *foreground_rgb = calloc(3, 1);
    uint8_t     *background_rgb = calloc(3, 1);
    uint8_t     draw_foreground, draw_background, *draw_foreground_rgb, *draw_background_rgb;
    bool        bold    = false;
    bool        blink   = false;
    bool        inverse = false;
    int32_t     x = 0, y = 0;
    int32_t     stored_x = -1, stored_y = -1;
    bool        pre_escaped = false;
    bool        escaped     = false;
    EscapeCodes *codes    = create_escape_codes();
    file                  = malloc(sizeof(TextArtFile));
    file->screen          = create_screen_with_palette_and_font(CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB, ANSI_PALETTE, CP437_8x16);
    file_ptr              = fopen(filename, "r");
    file->sauce           = get_sauce(file_ptr);
    file->length          = get_real_file_size(file_ptr, file->sauce);
    file->screen->columns = t_info_1(file->sauce, ANSI_DEFAULT_COLUMNS);
    use_sauce_ansiflags(file->screen, file->sauce);
    for(uint32_t i = 0; i < file->length; i += 1)
    {
        fread(&ascii_code, 1, 1, file_ptr);
        if(escaped)
        {
            switch(ascii_code)
            {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                add_numeric_to_escape_codes(codes, ascii_code);
                break;
                case 'A':
                if(codes->length == 0)
                {
                    codes->values[0] = 1;
                }
                y -= codes->values[0];
                check_limits(&x, &y, file->screen);
                escaped = false;
                break;
                case 'B':
                if(codes->length == 0)
                {
                    codes->values[0] = 1;
                }
                y += codes->values[0];
                check_limits(&x, &y, file->screen);
                escaped = false;
                break;
                case 'C':
                if(codes->length == 0)
                {
                    codes->values[0] = 1;
                }
                x += codes->values[0];
                check_limits(&x, &y, file->screen);
                escaped = false;
                break;
                case 'D':
                if(codes->length == 0)
                {
                    codes->values[0] = 1;
                }
                x -= codes->values[0];
                check_limits(&x, &y, file->screen);
                escaped = false;
                break;
                case 'H':
                if(codes->length == 0)
                {
                    codes->values[0] = 1;
                    codes->values[1] = 1;
                }
                else if(codes->length == 1)
                {
                    codes->values[1] = 1;
                }
                if(file->screen->rows < 25)
                {
                    y = codes->values[0] - 1;
                }
                else
                {
                    y = file->screen->rows - 25 + codes->values[0];
                }
                x = codes->values[1] - 1;
                check_limits(&x, &y, file->screen);
                escaped = false;
                break;
                case 'J':
                if(codes->length == 0)
                {
                    codes->values[0] = 0;
                }
                switch(codes->values[0])
                {
                    case 0:
                    clear_until_bottom_of_screen_from_current_position(file->screen, (uint32_t) y);
                    break;
                    case 1:
                    clear_until_top_of_screen_from_current_position(file->screen, (uint32_t) y);
                    break;
                    case 2:
                    clear_screen(file->screen);
                    x = 0;
                    y = 0;
                    break;
                    default:
                    break;
                }
                escaped = false;
                break;
                case 'K':
                if(codes->length == 0)
                {
                    clear_until_end_of_line_from_current_position(file->screen, (uint32_t) x, (uint32_t) y);
                }
                else if(codes->values[0] == 2)
                {
                    clear_entire_line_at_current_position(file->screen, (uint32_t) y);
                }
                escaped = false;
                break;
                case 'm':
                if(codes->length == 0)
                {
                    codes->values[0] = 0;
                    codes->length   = 1;
                }
                for(uint32_t i = 0; i < codes->length; i += 1)
                {
                    switch(codes->values[i])
                    {
                        case 0:
                        foreground = 7;
                        background = 0;
                        foreground_rgb_mode = false;
                        background_rgb_mode = false;
                        bold    = false;
                        blink   = false;
                        inverse = false;
                        break;
                        case 1:
                        bold = true;
                        foreground_rgb_mode = false;
                        break;
                        case 5:
                        blink = true;
                        break;
                        case 7:
                        inverse = true;
                        break;
                        case 22:
                        bold = false;
                        break;
                        case 25:
                        blink = false;
                        break;
                        case 27:
                        inverse = false;
                        break;
                        case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37:
                        foreground = codes->values[i] - 30;
                        foreground_rgb_mode = false;
                        break;
                        case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
                        background = codes->values[i] - 40;
                        background_rgb_mode = false;
                        break;
                        default:
                        break;
                    }
                }
                escaped = false;
                break;
                case 's':
                stored_x = x;
                stored_y = y;
                escaped = false;
                break;
                case 't':
                if(codes->length >= 4)
                {
                    switch(codes->values[0])
                    {
                        case 0:
                        memcpy(background_rgb, codes->values + 1, 3);
                        background_rgb_mode = true;
                        break;
                        case 1:
                        memcpy(foreground_rgb, codes->values + 1, 3);
                        foreground_rgb_mode = true;
                        break;
                        default:
                        break;
                    }
                }
                escaped = false;
                break;
                case 'u':
                if(stored_x != -1 && stored_y != -1)
                {
                    x = stored_x;
                    y = stored_y;
                }
                escaped = false;
                break;
                default:
                if(ascii_code >= '@' && ascii_code <= '~')
                {
                    escaped = false;
                }
                else
                {
                    append_escape_code(codes);
                }
                break;
            }
        }
        else
        {
            switch(ascii_code)
            {
                case '\n':
                y += 1;
                x  = 0;
                break;
                case '\r': case 26:
                pre_escaped = false;
                break;
                case 27:
                if(!pre_escaped)
                {
                    pre_escaped = true;
                }
                else
                {
                    goto LITERAL;
                }
                break;
                case '[':
                if(pre_escaped)
                {
                    pre_escaped = false;
                    escaped     = true;
                    start_new_escape_sequence(codes);
                }
                else
                {
                    goto LITERAL;
                }
                break;
                default:
                LITERAL:
                if(inverse)
                {
                    draw_foreground = background;
                    draw_background = foreground;
                    if(bold)
                    {
                        draw_background += 8;
                    }
                    if(blink)
                    {
                        draw_foreground += 8;
                    }
                    draw_foreground_rgb = background_rgb;
                    draw_background_rgb = foreground_rgb;
                }
                else
                {
                    draw_foreground = foreground;
                    draw_background = background;
                    if(bold)
                    {
                        draw_foreground += 8;
                    }
                    if(blink)
                    {
                        draw_background += 8;
                    }
                    draw_foreground_rgb = foreground_rgb;
                    draw_background_rgb = background_rgb;
                }
                put_character_and_attribute_pair_with_optional_rgb_on_screen(file->screen, (uint16_t *) &x, (uint16_t *) &y, ascii_code, (draw_background << 4) + draw_foreground, foreground_rgb_mode, draw_foreground_rgb, background_rgb_mode, draw_background_rgb);
                if((foreground_rgb_mode || background_rgb_mode) && !uses_rgb_colors)
                {
                    uses_rgb_colors = true;
                }
                break;
            }
        }
    }
    fclose(file_ptr);
    truncate_screen_data(file->screen);
    free(foreground_rgb);
    free(background_rgb);
    free_escape_codes(codes);
    if(!uses_rgb_colors)
    {
        strip_rgb_data(file->screen);
    }
    return file;
}
