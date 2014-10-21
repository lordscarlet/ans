#include <stdint.h>
#include <stdlib.h>

#include "text.h"
#include "utf8.h"
#include "../file/file.h"
#include "../file/formats/screen.h"
#include "../file/formats/palette.h"

void display_text_file_as_utf8(TextArtFile *file)
{
    uint8_t  ascii_code;
    uint32_t i = 0;
    switch(file->screen->type)
    {
        case CHARACTERS:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 1)
            {
                ascii_code = file->screen->data[i + 0];
                print_cp437_code(ascii_code);
            }
            printf("\n");
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 2)
            {
                ascii_code = file->screen->data[i + 0];
                print_cp437_code(ascii_code);
            }
            printf("\n");
        }
        break;
        case RGB_DATA:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 7)
            {
                ascii_code = file->screen->data[i + 0];
                print_cp437_code(ascii_code);
            }
            printf("\n");
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 10)
            {
                ascii_code = file->screen->data[i + 0];
                print_cp437_code(ascii_code);
            }
            printf("\n");
        }
        break;
    }
}

void print_ansi_escape_sequence(uint8_t foreground, uint8_t background)
{
    if(foreground >= 8)
    {
        printf("\x1b[1;%d;", foreground - 8 + 30);
    }
    else
    {
        printf("\x1b[0;%d;", foreground + 30);
    }
    if(background >= 8)
    {
        printf("5;%dm", background - 8 + 40);
    }
    else
    {
        printf("%dm", background + 40);
    }
}

void display_text_file_as_ansi(TextArtFile *file)
{
    uint8_t  ascii_code;
    uint32_t i = 0;
    uint8_t  foreground, background, *lookup_values, *foreground_rgb, *background_rgb;
    Palette  *ansi = get_preset_palette(ANSI_PALETTE);
    switch(file->screen->type)
    {
        case CHARACTERS:
        printf("\x1b[0m\n");
        display_text_file_as_utf8(file);
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        lookup_values = find_equivelant_colors(file->screen->palette, ansi);
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 2)
            {
                ascii_code = file->screen->data[i + 0];
                foreground = file->screen->data[i + 1] & 0xf;
                background = file->screen->data[i + 1] >> 4;
                print_ansi_escape_sequence(lookup_values[foreground], lookup_values[background]);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        free(lookup_values);
        break;
        case RGB_DATA:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 7)
            {
                ascii_code     = file->screen->data[i + 0];
                foreground_rgb = file->screen->data + i + 1;
                background_rgb = file->screen->data + i + 4;
                foreground = compare_rgb_with_palette(foreground_rgb, ansi);
                background = compare_rgb_with_palette(background_rgb, ansi);
                print_ansi_escape_sequence(foreground, background);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        lookup_values = find_equivelant_colors(file->screen->palette, ansi);
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 10)
            {
                ascii_code = file->screen->data[i + 0];
                if(file->screen->data[i + 2] == NON_RGB_ATTRIBUTE_DATA)
                {
                    foreground = file->screen->data[i + 1] & 0xf;
                }
                else
                {
                    foreground_rgb = file->screen->data + i + 3;
                    foreground = compare_rgb_with_palette(foreground_rgb, ansi);
                }
                if(file->screen->data[i + 6] == NON_RGB_ATTRIBUTE_DATA)
                {
                    background = file->screen->data[i + 1] >> 4;
                }
                else
                {
                    background_rgb = file->screen->data + i + 7;
                    background = compare_rgb_with_palette(background_rgb, ansi);
                }
                print_ansi_escape_sequence(foreground, background);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        free(lookup_values);
        break;
    }
    free_palette(ansi);
}

void print_xterm256_escape_sequence(uint8_t foreground, uint8_t background)
{
    printf("\x1b[38;5;%dm\x1b[48;5;%dm", foreground, background);
}

void display_text_file_as_xterm256(TextArtFile *file)
{
    uint8_t  ascii_code;
    uint32_t i = 0;
    uint8_t  foreground, background, *lookup_values, *foreground_rgb, *background_rgb;
    Palette  *xterm256 = get_preset_palette(XTERM256_PALETTE);
    switch(file->screen->type)
    {
        case CHARACTERS:
        printf("\x1b[0m\n");
        display_text_file_as_utf8(file);
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        lookup_values = find_equivelant_colors(file->screen->palette, xterm256);
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 2)
            {
                ascii_code = file->screen->data[i + 0];
                foreground = file->screen->data[i + 1] & 0xf;
                background = file->screen->data[i + 1] >> 4;
                print_xterm256_escape_sequence(lookup_values[foreground], lookup_values[background]);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        free(lookup_values);
        break;
        case RGB_DATA:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 7)
            {
                ascii_code     = file->screen->data[i + 0];
                foreground_rgb = file->screen->data + i + 1;
                background_rgb = file->screen->data + i + 4;
                foreground = compare_rgb_with_palette(foreground_rgb, xterm256);
                background = compare_rgb_with_palette(background_rgb, xterm256);
                print_xterm256_escape_sequence(foreground, background);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        lookup_values = find_equivelant_colors(file->screen->palette, xterm256);
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 10)
            {
                ascii_code = file->screen->data[i + 0];
                if(file->screen->data[i + 2] == NON_RGB_ATTRIBUTE_DATA)
                {
                    foreground = file->screen->data[i + 1] & 0xf;
                }
                else
                {
                    foreground_rgb = file->screen->data + i + 3;
                    foreground = compare_rgb_with_palette(foreground_rgb, xterm256);
                }
                if(file->screen->data[i + 6] == NON_RGB_ATTRIBUTE_DATA)
                {
                    background = file->screen->data[i + 1] >> 4;
                }
                else
                {
                    background_rgb = file->screen->data + i + 7;
                    background = compare_rgb_with_palette(background_rgb, xterm256);
                }
                print_xterm256_escape_sequence(foreground, background);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        free(lookup_values);
        break;
    }
    free_palette(xterm256);
}

void print_xterm24bit_sequence(uint8_t *foreground, uint8_t *background)
{
    printf("\x1b[38;2;%d;%d;%dm", foreground[0], foreground[1], foreground[2]);
    printf("\x1b[48;2;%d;%d;%dm", background[0], background[1], background[2]);
}

void display_text_file_as_xterm24bit(TextArtFile *file)
{
    uint8_t  ascii_code;
    uint32_t i = 0;
    uint8_t  foreground, background, *foreground_rgb, *background_rgb;
    switch(file->screen->type)
    {
        case CHARACTERS:
        display_text_file_as_utf8(file);
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 2)
            {
                ascii_code = file->screen->data[i + 0];
                foreground = file->screen->data[i + 1] & 0xf;
                background = file->screen->data[i + 1] >> 4;
                foreground_rgb = file->screen->palette->rgb_data + foreground * 3;
                background_rgb = file->screen->palette->rgb_data + background * 3;
                print_xterm24bit_sequence(foreground_rgb, background_rgb);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        break;
        case RGB_DATA:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 7)
            {
                ascii_code = file->screen->data[i + 0];
                print_xterm24bit_sequence(file->screen->data + i + 1, file->screen->data + i + 4);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 10)
            {
                ascii_code = file->screen->data[i + 0];
                if(file->screen->data[i + 2] == NON_RGB_ATTRIBUTE_DATA)
                {
                    foreground = file->screen->data[i + 1] & 0xf;
                    foreground_rgb = file->screen->palette->rgb_data + foreground * 3;
                }
                else
                {
                    foreground_rgb = file->screen->data + i + 3;
                }
                if(file->screen->data[i + 6] == NON_RGB_ATTRIBUTE_DATA)
                {
                    background = file->screen->data[i + 1] >> 4;
                    background_rgb = file->screen->palette->rgb_data + background * 3;
                }
                else
                {
                    background_rgb = file->screen->data + i + 7;
                }
                print_xterm24bit_sequence(foreground_rgb, background_rgb);
                print_cp437_code(ascii_code);
            }
            printf("\x1b[0m\n");
        }
        break;
    }
}

void display_files_as_text(char **filenames, uint32_t filenames_length, TextType type)
{
    TextArtFile *file;
    for(uint32_t i = 0; i < filenames_length; i += 1)
    {
        file = read_text_art_file(filenames[i]);
        if(file != NULL)
        {
            switch(type)
            {
                case TEXT_UTF8:
                display_text_file_as_utf8(file);
                break;
                case TEXT_ANSI:
                display_text_file_as_ansi(file);
                break;
                case TEXT_XTERM256:
                display_text_file_as_xterm256(file);
                break;
                case TEXT_XTERM24BIT:
                display_text_file_as_xterm24bit(file);
                break;
            }
            free_text_art_file(file);
        }
    }
}
