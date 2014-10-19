#include <stdio.h>
#include <stdlib.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint16_t ASCII_DEFAULT_COLUMNS = 80;

TextArtFile* load_ascii_file(char const *filename)
{
    TextArtFile *file;
    FILE        *file_ptr;
    uint8_t     ascii_code;
    uint16_t    x = 0;
    uint16_t    y = 0;
    file                  = malloc(sizeof(TextArtFile));
    file->screen          = create_screen_with_font(CHARACTERS, CP437_8x16);
    file->screen->palette = get_preset_palette(ASCII_PALETTE);
    file_ptr              = fopen(filename, "r");
    file->sauce           = get_sauce(file_ptr);
    file->length          = get_real_file_size(file_ptr, file->sauce);
    file->screen->columns = t_info_1(file->sauce, ASCII_DEFAULT_COLUMNS);
    use_sauce_ansiflags(file->screen, file->sauce);
    for(uint32_t i = 0; i < file->length; i += 1)
    {
        fread(&ascii_code, 1, 1, file_ptr);
        switch(ascii_code)
        {
            case '\n':
            x = 0;
            y += 1;
            break;
            case '\r':
            break;
            default:
            put_character_on_screen(file->screen, &x, &y, ascii_code);
        }
    }
    fclose(file_ptr);
    truncate_screen_data(file->screen);
    return file;
}
