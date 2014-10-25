#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "palette.h"
#include "palettes/ascii_palette.h"
#include "palettes/binary_palette.h"
#include "palettes/ansi_palette.h"
#include "palettes/xterm256_palette.h"

void generate_lab_values(uint8_t *rgb_data, double *lab_data, uint16_t palette_length)
{
    double red, green, blue, x, y, z;
    uint32_t palette_end = (uint32_t) palette_length * 3;
    for(uint16_t i = 0; i < palette_end; i += 3)
    {
        red   = (double) rgb_data[i + 0];
        green = (double) rgb_data[i + 1];
        blue  = (double) rgb_data[i + 2];
        red   = red / 255.0;
        green = green / 255.0;
        blue  = blue / 255.0;
        if(red > 0.04045)
        {
            red = pow((red + 0.055) / 1.055, 2.4);
        }
        else
        {
            red = red / 12.92;
        }
        if(green > 0.04045)
        {
            green = pow((green + 0.055) / 1.055, 2.4);
        }
        else
        {
            green = green / 12.92;
        }
        if(blue > 0.04045)
        {
            blue = pow((blue + 0.055) / 1.055, 2.4);
        }
        else
        {
            blue = blue / 12.92;
        }
        red = red * 100;
        green = green * 100;
        blue = blue * 100;
        x = red * 0.4124 + green * 0.3576 + blue * 0.1805;
        y = red * 0.2126 + green * 0.7152 + blue * 0.0722;
        z = red * 0.0193 + green * 0.1192 + blue * 0.9505;
        x = x / 95.047;
        y = y / 100.000;
        z = z / 108.883;
        if (x > 0.008856)
        {
            x = pow(x, 1.0 / 3.0);
        }
        else
        {
            x = (7.787 * x) + (16.0 / 116.0);
        }
        if (y > 0.008856)
        {
            y = pow(y, 1.0 / 3.0);
        }
        else
        {
            y = (7.787 * y) + (16.0 / 116.0);
        }
        if (z > 0.008856)
        {
            z = pow(z, 1.0 / 3.0);
        }
        else
        {
            z = (7.787 * z) + (16.0 / 116.0);
        }
        lab_data[i + 0] = (116.0 * y) - 16;
        lab_data[i + 1] = 500.0 * (x - y);
        lab_data[i + 2] = 200.0 * (y - z);
    }
}

uint16_t compare_lab_value_with_palette(double *lab_data, Palette *reference)
{
    double value, lowest;
    uint16_t match;
    uint32_t palette_end = (uint32_t) reference->length * 3;
    for(uint32_t i = 0, j = 0; i < palette_end; i += 3, j += 1)
    {
        value = sqrt(pow(lab_data[0] - reference->lab_data[i + 0], 2) + pow(lab_data[1] - reference->lab_data[i + 1], 2) + pow(lab_data[2] - reference->lab_data[i + 2], 2));
        if(i == 0 || value < lowest)
        {
            match = j;
            lowest = value;
        }
    }
    return match;
}

uint16_t compare_indexed_palette_with_palette(Palette *source, uint16_t index, Palette *reference)
{
    return compare_lab_value_with_palette(source->lab_data + ((uint32_t) index * 3), reference);
}

uint16_t compare_rgb_with_palette(uint8_t *rgb_data, Palette *reference)
{
    double lab_data[3];
    generate_lab_values(rgb_data, lab_data, 1);
    return compare_lab_value_with_palette(lab_data, reference);
}

uint8_t* find_equivelant_colors(Palette *palette, Palette *reference)
{
    uint8_t *lookup = malloc((size_t) palette->length);
    for(uint16_t i = 0; i < palette->length; i += 1)
    {
        lookup[i] = compare_indexed_palette_with_palette(palette, i, reference);
    }
    return lookup;
}

void generate_rgb_data(Palette *palette)
{
    uint32_t palette_end = (uint32_t) palette->length * 3;
    switch(palette->data_type)
    {
        case PAL_DATA_18BIT:
        for(uint32_t i = 0; i < palette_end; i += 1)
        {
            palette->rgb_data[i] = palette->data[i] << 2 | palette->data[i] >> 4;
        }
        break;
        case PAL_DATA_24BIT:
        memcpy(palette->rgb_data, palette->data, (size_t) palette_end);
        break;
    }
    generate_lab_values(palette->rgb_data, palette->lab_data, palette->length);
}

Palette* get_preset_palette(PaletteType type)
{
    Palette *palette;
    uint32_t palette_end;
    switch(type)
    {
        case ASCII_PALETTE:
        palette = malloc(sizeof(Palette));
        palette->data = ascii_palette;
        palette->data_type = ascii_palette_data_type;
        palette->length = ascii_palette_length;
        palette->name = ascii_palette_name;
        break;
        case BINARY_PALETTE:
        palette = malloc(sizeof(Palette));
        palette->data = binary_palette;
        palette->data_type = binary_palette_data_type;
        palette->length = binary_palette_length;
        palette->name = binary_palette_name;
        break;
        case ANSI_PALETTE:
        palette = malloc(sizeof(Palette));
        palette->data = ansi_palette;
        palette->data_type = ansi_palette_data_type;
        palette->length = ansi_palette_length;
        palette->name = ansi_palette_name;
        break;
        case XTERM256_PALETTE:
        palette = malloc(sizeof(Palette));
        palette->data = xterm256_palette;
        palette->data_type = xterm256_palette_data_type;
        palette->length = xterm256_palette_length;
        palette->name = xterm256_palette_name;
        break;
        default:
        return NULL;
    }
    palette->type = type;
    palette_end = (size_t) palette->length * 3;
    palette->rgb_data = malloc(sizeof(uint8_t) * palette_end);
    palette->lab_data = malloc(sizeof(double) * palette_end);
    generate_rgb_data(palette);
    return palette;
}

Palette* create_new_palette(uint16_t palette_length, PaletteDataType data_type)
{
    size_t palette_end = (size_t) palette_length * 3;
    Palette *palette = malloc(sizeof(Palette));
    palette->type = CUSTOM_PALETTE;
    palette->length = palette_length;
    palette->data = malloc(sizeof(uint8_t) * palette_end);
    palette->data_type = data_type;
    palette->rgb_data = malloc(sizeof(uint8_t) * palette_end);
    palette->lab_data = malloc(sizeof(double) * palette_end);
    palette->name = NULL;
    return palette;
}

Palette* load_palette(FILE *file_ptr, uint16_t palette_length, PaletteDataType data_type)
{
    Palette *palette = create_new_palette(palette_length, data_type);
    fread(palette->data, 1, (size_t) palette_length * 3, file_ptr);
    generate_rgb_data(palette);
    return palette;
}

void free_palette(Palette *palette)
{
    if(palette != NULL)
    {
        if(palette->type == CUSTOM_PALETTE)
        {
            free(palette->data);
        }
        free(palette->rgb_data);
        free(palette->lab_data);
        free(palette);
    }
}

void debug_palette_values(Palette *palette)
{
    uint32_t palette_end = (uint32_t) palette->length * 3;
    printf("Palette length: %d\n", palette->length);
    printf("Palette values: ");
    for(uint16_t i = 0; i < palette_end; i += 3) {
        printf("(%d, %d, %d)", palette->data[i], palette->data[i + 1], palette->data[i + 2]);
        if(i < palette_end - 3)
        {
            printf(", ");
        }
    }
    printf("\n");
}

void debug_palette(Palette *palette)
{
    printf("Palette: ");
    switch(palette->type)
    {
        case CUSTOM_PALETTE:
        printf("Included in file\n");
        debug_palette_values(palette);
        break;
        case ASCII_PALETTE:
        printf("Default ASCII palette\n");
        break;
        case BINARY_PALETTE:
        printf("Default binary-ordered palette\n");
        debug_palette_values(palette);
        break;
        case ANSI_PALETTE:
        printf("Default ANSI-ordered palette\n");
        debug_palette_values(palette);
        break;
        case XTERM256_PALETTE:
        printf("Default XTerm-256-ordered palette\n");
        debug_palette_values(palette);
        break;
    }
}