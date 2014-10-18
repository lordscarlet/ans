#ifndef FONT_H_F21F0D19
#define FONT_H_F21F0D19

#include <stdint.h>

typedef enum {
    CUSTOM_FONT,
    IBM_VGA_8x16
} FontType;

typedef struct {
    FontType type;
    uint8_t  width;
    uint8_t  height;
    uint16_t length;
    uint8_t  *bytes;
    uint8_t  *bits;
    char     *name;
} Font;

Font* get_preset_font(FontType type);

Font* load_font(uint8_t height, uint16_t length, FILE *file_ptr);

Font* load_8x16x256_font(FILE *file_ptr);

void free_font(Font *font);

void debug_font(Font *font);

#endif /* end of include guard: FONT_H_F21F0D19 */
