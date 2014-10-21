#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint32_t SCREEN_CHUNK_LENGTH = 10000;

Screen* create_screen(ScreenType screen_type)
{
    Screen *screen              = malloc(sizeof(Screen));
    screen->type                = screen_type;
    screen->palette             = NULL;
    screen->font                = NULL;
    screen->columns             = 0;
    screen->rows                = 0;
    screen->data                = NULL;
    screen->non_blink           = false;
    screen->letter_spacing      = false;
    screen->legacy_aspect_ratio = false;
    return screen;
}

Screen* create_screen_with_font(ScreenType screen_type, FontType font_type)
{
    Screen *screen  = create_screen(screen_type);
    screen->font    = get_preset_font(font_type);
    return screen;
}

Screen* create_screen_with_palette_and_font(ScreenType screen_type, PaletteType palette_type, FontType font_type)
{
    Screen *screen  = create_screen_with_font(screen_type, font_type);
    screen->palette = get_preset_palette(palette_type);
    return screen;
}

void load_screen(Screen *screen, FILE *file_ptr)
{
    switch(screen->type)
    {
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        screen->length   = screen->columns * screen->rows * 2;
        screen->data     = malloc(screen->length);
        fread(screen->data, 1, screen->length, file_ptr);
        break;
        default:
        break;
    }
}

void copy_screen_from_bytes(Screen *screen, uint8_t *bytes)
{
    switch(screen->type)
    {
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        screen->length   = screen->columns * screen->rows * 2;
        screen->data     = malloc(screen->length);
        memcpy(screen->data, bytes, screen->length);
        break;
        default:
        break;
    }
}

void use_sauce_ansiflags(Screen *screen, Sauce *sauce)
{
    if(sauce != NULL)
    {
        screen->non_blink           = sauce->non_blink;
        screen->letter_spacing      = (sauce->letter_spacing == NINE_PIXEL);
        screen->legacy_aspect_ratio = (sauce->aspect_ratio   == LEGACY);
    }
}

void debug_screen(Screen *screen)
{
    if(screen->data != NULL)
    {
        printf("Screen type: ");
        switch(screen->type)
        {
            case CHARACTERS:
            printf("ASCII characters\n");
            break;
            case CHARACTER_AND_ATTRIBUTE_PAIR:
            printf("Character and attribute byte-pair\n");
            break;
            case RGB_DATA:
            printf("RGB data\n");
            break;
            case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
            printf("Character and attribute byte-pair with 24bit color escape sequences\n");
            break;
        }
        printf("Screen columns: %i\n", screen->columns);
        printf("Screen rows: %i\n",    screen->rows);
        if(screen->palette != NULL)
        {
            debug_palette(screen->palette);
        }
        if(screen->font != NULL)
        {
            debug_font(screen->font);
        }
        printf("Screen non-blink (iCE color) mode: ");
        if(screen->non_blink)
        {
            printf("Yes\n");
        }
        else
        {
            printf("No\n");
        }
        printf("Screen letter spacing: ");
        if(screen->letter_spacing)
        {
            printf("Yes\n");
        }
        else
        {
            printf("No\n");
        }
        printf("Screen legacy aspect ratio: ");
        if(screen->legacy_aspect_ratio)
        {
            printf("Yes\n");
        }
        else
        {
            printf("No\n");
        }
    }
    else
    {
        printf("No screen data\n");
    }
}

void free_screen(Screen *screen)
{
    if(screen != NULL)
    {
        if(screen->palette != NULL)
        {
            free_palette(screen->palette);
        }
        if(screen->font != NULL)
        {
            free_font(screen->font);
        }
        if(screen->data != NULL)
        {
            free(screen->data);
        }
        free(screen);
    }
}

void put_character_and_attribute_pair_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t attribute)
{
    uint32_t data_position, new_length;
    void     *new_data;
    switch(screen->type)
    {
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        if(screen->data == NULL)
        {
            screen->length = SCREEN_CHUNK_LENGTH * screen->columns * 2;
            screen->data   = calloc(screen->length, 1);
        }
        data_position = (*y * screen->columns + *x) * 2;
        if(data_position >= screen->length)
        {
            new_length = (*y + SCREEN_CHUNK_LENGTH) * screen->columns * 2;
            new_data   = calloc(new_length, 1);
            memcpy(new_data, screen->data, (size_t) screen->length);
            free(screen->data);
            screen->length = new_length;
            screen->data   = new_data;
        }
        screen->data[data_position + 0] = ascii_code;
        screen->data[data_position + 1] = attribute;
        *x += 1;
        if(*x == screen->columns)
        {
            *x = 0;
            *y += 1;
        }
        if(*y + 1 > screen->rows)
        {
            screen->rows = *y + 1;
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        if(screen->data == NULL)
        {
            screen->length = SCREEN_CHUNK_LENGTH * screen->columns * 10;
            screen->data   = calloc(screen->length, 1);
        }
        data_position = (*y * screen->columns + *x) * 10;
        if(data_position >= screen->length)
        {
            new_length = (*y + SCREEN_CHUNK_LENGTH) * screen->columns * 10;
            new_data   = calloc(new_length, 1);
            memcpy(new_data, screen->data, (size_t) screen->length);
            free(screen->data);
            screen->length = new_length;
            screen->data   = new_data;
        }
        screen->data[data_position + 0] = ascii_code;
        screen->data[data_position + 1] = attribute;
        screen->data[data_position + 2] = NON_RGB_ATTRIBUTE_DATA;
        *x += 1;
        if(*x == screen->columns)
        {
            *x = 0;
            *y += 1;
        }
        if(*y + 1 > screen->rows)
        {
            screen->rows = *y + 1;
        }
        break;
        default:
        break;
    }
}

void put_character_and_attribute_pair_with_optional_rgb_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t attribute, bool foreground_rgb, uint8_t *foreground, bool background_rgb, uint8_t *background)
{
    uint32_t data_position, new_length;
    void     *new_data;
    if(screen->data == NULL)
    {
        screen->length = SCREEN_CHUNK_LENGTH * screen->columns * 10;
        screen->data   = calloc(screen->length, 1);
    }
    data_position = (*y * screen->columns + *x) * 10;
    if(data_position >= screen->length)
    {
        new_length = (*y + SCREEN_CHUNK_LENGTH) * screen->columns * 10;
        new_data   = calloc(new_length, 1);
        memcpy(new_data, screen->data, (size_t) screen->length);
        free(screen->data);
        screen->length = new_length;
        screen->data   = new_data;
    }
    screen->data[data_position + 0] = ascii_code;
    screen->data[data_position + 1] = attribute;
    if(foreground_rgb)
    {
        screen->data[data_position + 2] = RGB_ATTRIBUTE_DATA;
        memcpy(screen->data + data_position + 3, foreground, 3);
    }
    else
    {
        screen->data[data_position + 2] = NON_RGB_ATTRIBUTE_DATA;
    }
    if(background_rgb)
    {
        screen->data[data_position + 6] = RGB_ATTRIBUTE_DATA;
        memcpy(screen->data + data_position + 7, background, 3);
    }
    else
    {
        screen->data[data_position + 6] = NON_RGB_ATTRIBUTE_DATA;
    }
    *x += 1;
    if(*x == screen->columns)
    {
        *x = 0;
        *y += 1;
    }
    if(*y + 1 > screen->rows)
    {
        screen->rows = *y + 1;
    }
}

void strip_rgb_data(Screen *screen)
{
    uint8_t  *new_data = malloc(screen->columns * screen->rows * 2);
    uint32_t current_screen_length = screen->columns * screen->rows * 10;
    for(uint32_t i = 0, j = 0; i < current_screen_length; i += 10, j += 2)
    {
        new_data[j + 0] = screen->data[i + 0];
        new_data[j + 1] = screen->data[i + 1];
    }
    free(screen->data);
    screen->data = new_data;
    screen->type = CHARACTER_AND_ATTRIBUTE_PAIR;
}

void put_rgb_data_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code, uint8_t *foreground, uint8_t *background)
{
    uint32_t data_position, new_length;
    void     *new_data;
    if(screen->data == NULL)
    {
        screen->length = SCREEN_CHUNK_LENGTH * screen->columns * 7;
        screen->data   = calloc(screen->length, 1);
    }
    data_position = (*y * screen->columns + *x) * 7;
    if(data_position >= screen->length)
    {
        new_length = (*y + SCREEN_CHUNK_LENGTH) * screen->columns * 7;
        new_data   = calloc(new_length, 1);
        memcpy(new_data, screen->data, (size_t) screen->length);
        free(screen->data);
        screen->length = new_length;
        screen->data   = new_data;
    }
    screen->data[data_position + 0] = ascii_code;
    memcpy(screen->data + data_position + 1, foreground, 3);
    memcpy(screen->data + data_position + 4, background, 3);
    *x += 1;
    if(*x == screen->columns)
    {
        *x = 0;
        *y += 1;
    }
    if(*y + 1 > screen->rows)
    {
        screen->rows = *y + 1;
    }
}

void put_character_on_screen(Screen *screen, uint16_t *x, uint16_t *y, uint8_t ascii_code)
{
    uint32_t data_position, new_length;
    void     *new_data;
    if(screen->data == NULL)
    {
        screen->length = SCREEN_CHUNK_LENGTH * screen->columns;
        screen->data   = calloc(screen->length, 1);
    }
    data_position = (*y * screen->columns + *x);
    if(data_position >= screen->length)
    {
        new_length = (*y + SCREEN_CHUNK_LENGTH) * screen->columns;
        new_data   = calloc(new_length, 1);
        memcpy(new_data, screen->data, (size_t) screen->length);
        free(screen->data);
        screen->length = new_length;
        screen->data   = new_data;
    }
    screen->data[data_position] = ascii_code;
    *x += 1;
    if(*x == screen->columns)
    {
        *x = 0;
        *y += 1;
    }
    if(*y + 1 > screen->rows)
    {
        screen->rows = *y + 1;
    }
}

void clear_screen(Screen *screen)
{
    free(screen->data);
    screen->data = NULL;
    screen->rows = 0;
}

void clear_until_end_of_line_from_current_position(Screen *screen, uint16_t x, uint16_t y)
{
    uint32_t screen_position;
    switch(screen->type)
    {
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        screen_position = (y * screen->columns + x) * 10;
        for(uint16_t i = x; i < screen->columns; i += 1, screen_position += 10)
        {
            screen->data[screen_position + 0] = 0;
            screen->data[screen_position + 1] = 0;
            screen->data[screen_position + 2] = NON_RGB_ATTRIBUTE_DATA;
            screen->data[screen_position + 6] = NON_RGB_ATTRIBUTE_DATA;
        }
        break;
        default:
        break;
    }
}

void clear_entire_line_at_current_position(Screen *screen, uint16_t y)
{
    uint16_t x = 0;
    clear_until_end_of_line_from_current_position(screen, x, y);
}

void clear_until_top_of_screen_from_current_position(Screen *screen, uint16_t y)
{
    for(int32_t i = y; i >= screen->rows - 25; i -= 1)
    {
        clear_entire_line_at_current_position(screen, (uint16_t) i);
    }
}

void clear_until_bottom_of_screen_from_current_position(Screen *screen, uint16_t y)
{
    for(uint16_t i = y; i < screen->rows; i += 1)
    {
        clear_entire_line_at_current_position(screen, i);
    }
}

void truncate_screen_data(Screen *screen)
{
    switch(screen->type)
    {
        case CHARACTERS:
        screen->length = screen->columns * screen->rows;
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        screen->length = screen->columns * screen->rows * 2;
        break;
        case RGB_DATA:
        screen->length = screen->columns * screen->rows * 7;
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        screen->length = screen->columns * screen->rows * 10;
        break;
    }
    screen->data = realloc(screen->data, (size_t) screen->length);
}

uint16_t get_actual_columns(Screen *screen)
{
    uint16_t x     = 0;
    uint16_t y     = 0;
    uint16_t max_x = 0;
    switch(screen->type)
    {
        case CHARACTERS:
        for(uint32_t i = 0; y < screen->rows; y += 1)
        {
            for(x = 0; x < screen->columns; x += 1, i += 1)
            {
                if(screen->data[i] != 0 && x > max_x)
                {
                    max_x = x;
                }
            }
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        for(uint32_t i = 0; y < screen->rows; y += 1)
        {
            for(x = 0; x < screen->columns; x += 1, i += 2)
            {
                if(screen->data[i] != 0 && x > max_x)
                {
                    max_x = x;
                }
            }
        }
        break;
        case RGB_DATA:
        for(uint32_t i = 0; y < screen->rows; y += 1)
        {
            for(x = 0; x < screen->columns; x += 1, i += 7)
            {
                if(screen->data[i] != 0 && x > max_x)
                {
                    max_x = x;
                }
            }
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        for(uint32_t i = 0; y < screen->rows; y += 1)
        {
            for(x = 0; x < screen->columns; x += 1, i += 10)
            {
                if(screen->data[i] != 0 && x > max_x)
                {
                    max_x = x;
                }
            }
        }
        break;
    }
    return max_x + 1;
}

void crop_horizontally(Screen *screen, uint16_t from, uint16_t new_columns)
{
    uint8_t *new_data;
    switch(screen->type)
    {
        case CHARACTERS:
        screen->length = new_columns * screen->rows;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y, screen->data + screen->columns * y + from, new_columns);
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        screen->length = new_columns * screen->rows * 2;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y * 2, screen->data + (screen->columns * y + from) * 2, new_columns * 2);
        }
        break;
        case RGB_DATA:
        screen->length = new_columns * screen->rows * 7;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y * 7, screen->data + (screen->columns * y + from) * 7, new_columns * 7);
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        screen->length = new_columns * screen->rows * 10;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y * 10, screen->data + (screen->columns * y + from) * 10, new_columns * 10);
        }
        break;
    }
    free(screen->data);
    screen->columns = new_columns;
    screen->data    = new_data;
}

void trim_columns(Screen *screen, uint16_t new_columns)
{
    uint8_t *new_data;
    switch(screen->type)
    {
        case CHARACTERS:
        screen->length = new_columns * screen->rows;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y, screen->data + screen->columns * y, screen->columns);
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR:
        screen->length = new_columns * screen->rows * 2;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y * 2, screen->data + screen->columns * y * 2, screen->columns * 2);
        }
        break;
        case RGB_DATA:
        screen->length = new_columns * screen->rows * 7;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y * 7, screen->data + screen->columns * y * 7, screen->columns * 7);
        }
        break;
        case CHARACTER_AND_ATTRIBUTE_PAIR_WITH_RGB:
        screen->length = new_columns * screen->rows * 10;
        new_data = malloc((size_t) screen->length);
        for(uint16_t y = 0; y < screen->rows; y += 1)
        {
            memcpy(new_data + new_columns * y * 10, screen->data + screen->columns * y * 10, screen->columns * 10);
        }
        break;
    }
    free(screen->data);
    screen->columns = new_columns;
    screen->data    = new_data;
}