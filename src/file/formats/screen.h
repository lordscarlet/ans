#ifndef SCREEN_H_43004382
#define SCREEN_H_43004382

#include <stdbool.h>

#include "palette.h"
#include "font.h"
#include "../sauce.h"
#include "../../image/canvas.h"

typedef enum {
    CHARACTERS,
    CHARACTER_AND_ATTRIBUTE_PAIR,
    RGB_DATA
} ScreenType;

typedef struct {
    ScreenType type;
    Palette    *palette;
    Font       *font;
    uint16_t   columns, rows;
    uint8_t    *data;
    uint32_t   length;
    bool       non_blink, letter_spacing, legacy_aspect_ratio;
} Screen;

Screen* create_screen(ScreenType screen_type);

Screen* create_screen_with_font(ScreenType screen_type, FontType font_type);

Screen* create_screen_with_palette_and_font(ScreenType screen_type, PaletteType palette_type, FontType font_type);

void load_screen(Screen *screen, FILE *file_ptr);

void use_sauce_ansiflags(Screen *screen, Sauce *sauce);

void debug_screen(Screen *screen);

void free_screen(Screen *screen);

void put_character_and_attribute_pair_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t attribute);

void put_rgb_data_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t *foreground, uint8_t *background);

void put_character_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code);

void clear_screen(Screen *screen);

void truncate_screen_data(Screen *screen);

Canvas* screen_to_canvas(Screen *screen);

uint16_t get_actual_columns(Screen *screen);

void change_columns(Screen *screen, uint16_t new_columns);

#endif /* end of include guard: SCREEN_H_43004382 */
