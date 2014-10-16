#ifndef SAUCE_H_B89B775F
#define SAUCE_H_B89B775F

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    NO_LETTER_SPACE_PREFERENCE,
    EIGHT_PIXEL,
    NINE_PIXEL,
    INVALID_LETTER_SPACE
} LetterSpace;

typedef enum {
    NO_ASPECT_RATIO_PREFERENCE,
    LEGACY,
    MODERN,
    INVALID_ASPECT_RATIO
} AspectRatio;

typedef struct {
    char        title[35];
    char        author[20];
    char        group[20];
    char        date[8];
    uint32_t    file_size;
    uint8_t     data_type;
    uint8_t     file_type;
    uint16_t    t_info_1;
    uint16_t    t_info_2;
    uint16_t    t_info_3;
    uint16_t    t_info_4;
    uint8_t     comments;
    bool        non_blink;
    LetterSpace letter_spacing;
    AspectRatio aspect_ratio;
    char t_info_s[22];
} Sauce;

Sauce* get_sauce(FILE *file_ptr);

void debug_sauce(Sauce *sauce);

uint32_t get_actual_file_size(FILE *file_ptr, Sauce *sauce);

#endif /* end of include guard: SAUCE_H_B89B775F */
