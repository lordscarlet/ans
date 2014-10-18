#ifndef FONT_H_F21F0D19
#define FONT_H_F21F0D19

#include <stdint.h>

typedef enum {
    CUSTOM_FONT,
    CP437_8x16
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

Font* create_and_load_font(uint8_t height, uint16_t length, FILE *file_ptr);

Font* create_font_from_bytes(uint8_t height, uint16_t length, uint8_t *bytes);

Font* create_and_load_8x16x256_font(FILE *file_ptr);

void free_font(Font *font);

void debug_font(Font *font);

#endif /* end of include guard: FONT_H_F21F0D19 */
