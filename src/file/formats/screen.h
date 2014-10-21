#ifndef SCREEN_H_43004382
#define SCREEN_H_43004382

#include <stdbool.h>

#include "palette.h"
#include "font.h"
#include "../sauce.h"

typedef enum {
    CHARACTERS,
    CHARACTER_AND_ATTRIBUTE_PAIR,
    RGB_DATA,
    CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB
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

enum {
    NON_RGB_ATTRIBUTE_DATA,
    RGB_ATTRIBUTE_DATA
};

Screen* create_screen(ScreenType screen_type);

Screen* create_screen_with_font(ScreenType screen_type, FontType font_type);

Screen* create_screen_with_palette_and_font(ScreenType screen_type, PaletteType palette_type, FontType font_type);

void load_screen(Screen *screen, FILE *file_ptr);

void copy_screen_from_bytes(Screen *screen, uint8_t *bytes);

void use_sauce_ansiflags(Screen *screen, Sauce *sauce);

void debug_screen(Screen *screen);

void free_screen(Screen *screen);

void put_character_and_attribute_pair_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t attribute);

void put_rgb_data_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t *foreground, uint8_t *background);

void put_character_and_attribute_pair_with_optional_rgb_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t attribute, bool foreground_rgb, uint8_t *foreground, bool background_rgb, uint8_t *background);

void strip_rgb_data(Screen *screen);

void put_character_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code);

void clear_screen(Screen *screen);

void clear_until_end_of_line_from_current_position(Screen *screen, uint16_t x, uint16_t y);

void clear_entire_line_at_current_position(Screen *screen, uint16_t y);

void clear_until_top_of_screen_from_current_position(Screen *screen, uint16_t y);

void clear_until_bottom_of_screen_from_current_position(Screen *screen, uint16_t y);

void truncate_screen_data(Screen *screen);

uint16_t get_actual_columns(Screen *screen);

void trim_columns(Screen *screen, uint16_t new_columns);

void crop_horizontally(Screen *screen, uint16_t from, uint16_t length);

#endif /* end of include guard: SCREEN_H_43004382 */
