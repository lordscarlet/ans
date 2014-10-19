#include <stdlib.h>
#include <stdio.h>

#include "palette.h"
#include "palettes/ascii_palette.h"
#include "palettes/binary_palette.h"
#include "palettes/ansi_palette.h"

void generate_rgb_data(Palette *palette)
{
    for(uint8_t i = 0; i < 48; i += 1)
    {
        palette->rgb_bytes[i] = palette->bytes[i] << 2 | palette->bytes[i] >> 4;
    }
}

Palette* get_preset_palette(PaletteType type)
{
    Palette *palette = malloc(sizeof(Palette));
    palette->type    = type;
    switch(type)
    {
        case ASCII_PALETTE:
        palette->bytes = ascii_palette;
        generate_rgb_data(palette);
        return palette;
        case BINARY_PALETTE:
        palette->bytes = binary_palette;
        generate_rgb_data(palette);
        return palette;
        case ANSI_PALETTE:
        palette->bytes = ansi_palette;
        generate_rgb_data(palette);
        return palette;
        default:
        free(palette);
        return NULL;
    }
}

Palette* create_new_palette()
{
    Palette *palette = malloc(sizeof(Palette));
    palette->type    = CUSTOM_PALETTE;
    palette->bytes   = malloc(48);
    return palette;
}

Palette* load_palette(FILE *file_ptr)
{
    Palette *palette = create_new_palette();
    fread(palette->bytes, 1, 48, file_ptr);
    generate_rgb_data(palette);
    return palette;
}

void free_palette(Palette *palette)
{
    if(palette != NULL)
    {
        if(palette->type == CUSTOM_PALETTE)
        {
            free(palette->bytes);
        }
        free(palette);
    }
}

void debug_palette(Palette *palette)
{
    printf("Palette: ");
    switch(palette->type)
    {
        case CUSTOM_PALETTE:
        printf("Included in file\n");
        printf("Palette values: ");
        for(size_t i = 0; i < 48; i += 3) {
            printf("(%d, %d, %d)", palette->bytes[i], palette->bytes[i + 1], palette->bytes[i + 2]);
            if(i < 45)
            {
                printf(", ");
            }
        }
        printf("\n");
        break;
        case ASCII_PALETTE:
        printf("Default ASCII palette\n");
        break;
        case BINARY_PALETTE:
        printf("Default binary-ordered palette\n");
        break;
        case ANSI_PALETTE:
        printf("Default ANSI-ordered palette\n");
        break;
    }
}