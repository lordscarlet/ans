#ifndef RENDERER_H_F7AAB97D
#define RENDERER_H_F7AAB97D

void convert_palette(uint8_t *palette_bytes, uint8_t *palette_rgb);

void font_bytes_to_bits(uint8_t *font_bytes, uint8_t font_height, uint8_t *font_bits);

void draw_glyph(Canvas *canvas, uint8_t ascii_code, uint8_t foreground, uint8_t background, uint16_t x, uint16_t y, uint8_t *palette_rgb, uint8_t *font_bits, uint8_t font_width, uint8_t font_height);

#endif /* end of include guard: RENDERER_H_F7AAB97D */