#ifndef TEXT_H_BB084655
#define TEXT_H_BB084655

#include <stdint.h>

typedef enum {
    TEXT_UTF8,
    TEXT_ANSI,
    TEXT_XTERM256,
    TEXT_XTERM24BIT
} TextType;

void display_files_as_text(char **filenames, uint32_t filenames_length, TextType type);

#endif /* end of include guard: TEXT_H_BB084655 */
