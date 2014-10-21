#include <stdlib.h>
#include <stdio.h>

#include "../file/file.h"
#include "../file/formats/screen.h"

void print_normal_ansi_escape_sequence(uint8_t foreground, uint8_t background, FILE *out)
{
    if(foreground >= 8)
    {
        fprintf(out, "\x1b[1;%d;", foreground - 8 + 30);
    }
    else
    {
        fprintf(out, "\x1b[0;%d;", foreground + 30);
    }
    if(background >= 8)
    {
        fprintf(out, "5;%dm", background - 8 + 40);
    }
    else
    {
        fprintf(out, "%dm", background + 40);
    }
}

void print_ansi24bit_escape_sequence(uint8_t *foreground, uint8_t *background, FILE *out)
{
    fprintf(out, "\x1b[1;%d;%d;%dt", foreground[0], foreground[1], foreground[2]);
    fprintf(out, "\x1b[0;%d;%d;%dt", background[0], background[1], background[2]);
}

void output_file_as_ansi24bit(TextArtFile *file, FILE *out)
{
    uint8_t  ascii_code;
    uint32_t i = 0;
    uint8_t  foreground, background, *lookup_values, *foreground_rgb, *background_rgb;
    Palette  *ansi = get_preset_palette(ANSI_PALETTE);
    printf("\x1b[?33h");
    switch(file->screen->type)
    {
        case CHARACTERS:
        fprintf(out, "\x1b[0m\n");
        for(uint16_t y = 0; y < file->screen->rows; y += 1)
        {
            for(uint16_t x = 0; x < file->screen->columns; x += 1, i += 1)
            {
                ascii_code = file->screen->data[i + 0];
                putc(ascii_code, out);
            }
        }
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
                foreground_rgb = file->screen->palette->rgb_data + foreground * 3;
                background_rgb = file->screen->palette->rgb_data + background * 3;
                print_normal_ansi_escape_sequence(lookup_values[foreground], lookup_values[background], out);
                print_ansi24bit_escape_sequence(foreground_rgb, background_rgb, out);
                putc(ascii_code, out);
            }
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
                print_normal_ansi_escape_sequence(foreground, background, out);
                print_ansi24bit_escape_sequence(foreground_rgb, background_rgb, out);
                putc(ascii_code, out);
            }
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
                    foreground_rgb = file->screen->palette->rgb_data + foreground * 3;
                }
                else
                {
                    foreground_rgb = file->screen->data + i + 3;
                    foreground = compare_rgb_with_palette(foreground_rgb, ansi);
                }
                if(file->screen->data[i + 6] == NON_RGB_ATTRIBUTE_DATA)
                {
                    background = file->screen->data[i + 1] >> 4;
                    background_rgb = file->screen->palette->rgb_data + background * 3;
                }
                else
                {
                    background_rgb = file->screen->data + i + 7;
                    background = compare_rgb_with_palette(background_rgb, ansi);
                }
                print_normal_ansi_escape_sequence(foreground, background, out);
                print_ansi24bit_escape_sequence(foreground_rgb, background_rgb, out);
                putc(ascii_code, out);
            }
        }
        free(lookup_values);
        break;
    }
    free_palette(ansi);
}

void ans24_main(char **args, uint32_t args_length)
{
    TextArtFile *file;
    FILE *out;
    uint16_t new_columns;
    file = read_text_art_file(args[0]);
    if(file != NULL)
    {
        if(file->screen->columns > 80)
        {
            if(args_length > 1)
            {
                new_columns = (uint16_t) strtol(args[1], NULL, 10);
            }
            else
            {
                new_columns = 0;
            }
            crop_horizontally(file->screen, new_columns, 80);
        }
        out = fopen("out.ans", "w");
        output_file_as_ansi24bit(file, out);
        fclose(out);
        free_text_art_file(file);
    }
}