#ifndef PALETTE_H_4C592A4D
#define PALETTE_H_4C592A4D

#include <stdint.h>
#include <stdio.h>

typedef enum {
    CUSTOM_PALETTE,
    ASCII_PALETTE,
    BINARY_PALETTE
} PaletteType;

typedef struct {
    PaletteType type;
    uint8_t     *bytes;
    uint8_t     rgb_bytes[48];
} Palette;

void generate_rgb_data(Palette *palette);

Palette* get_preset_palette(PaletteType type);

Palette* create_new_palette();

Palette* load_palette(FILE *file_ptr);

void free_palette(Palette *palette);

void debug_palette(Palette *palette);

#endif /* end of include guard: PALETTE_H_4C592A4D */
