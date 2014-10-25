#ifndef PALETTE_H_4C592A4D
#define PALETTE_H_4C592A4D

#include <stdint.h>
#include <stdio.h>

typedef enum {
    CUSTOM_PALETTE,
    ASCII_PALETTE,
    BINARY_PALETTE,
    ANSI_PALETTE,
    XTERM256_PALETTE
} PaletteType;

typedef enum {
    PAL_DATA_18BIT,
    PAL_DATA_24BIT
} PaletteDataType;

typedef struct {
    PaletteType type;
    uint8_t *data;
    PaletteDataType data_type;
    uint8_t *rgb_data;
    double *lab_data;
    uint16_t length;
    char *name;
} Palette;

void generate_rgb_data(Palette *palette);

uint16_t compare_indexed_palette_with_palette(Palette *source, uint16_t index, Palette *reference);

uint16_t compare_rgb_with_palette(uint8_t *rgb_data, Palette *reference);

uint8_t* find_equivelant_colors(Palette *palette, Palette *reference);

Palette* get_preset_palette(PaletteType type);

Palette* create_new_palette(uint16_t palette_length, PaletteDataType data_type);

Palette* load_palette(FILE *file_ptr, uint16_t palette_length, PaletteDataType data_type);

void free_palette(Palette *palette);

void debug_palette(Palette *palette);

#endif /* end of include guard: PALETTE_H_4C592A4D */
