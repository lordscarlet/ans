#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "../file.h"
#include "screen.h"
#include "palette.h"
#include "font.h"
#include "../sauce.h"

uint8_t  ANSIEDIT_LZ77_COMPRESSION = 1;
uint16_t ANSIEDIT_PALETTE_LENGTH   = 16;

typedef struct {
    uint8_t  magic[4];
    uint32_t block_length;
    uint32_t length;
    uint8_t  *bytes;
} Block;

uint8_t* decompress_ansiedit_block(uint8_t *input)
{
    uint8_t  *output;
    uint8_t  pointer_length_width;
    uint16_t input_pointer, pointer_length, pointer_pos, pointer_length_mask;
    uint32_t compressed_pointer, coding_pos, pointer_offset, output_length;
    output_length        = *((uint32_t *) input);
    output               = malloc(output_length);
    pointer_length_width = input[4];
    compressed_pointer   = 5;
    pointer_length_mask  = pow(2, pointer_length_width) - 1;
    for(coding_pos = 0; coding_pos < output_length; coding_pos += 1)
    {
        input_pointer = *((uint16_t *) (input + compressed_pointer));
        compressed_pointer += 2;
        pointer_pos = input_pointer >> pointer_length_width;
        if(pointer_pos > 0)
        {
            pointer_length = (input_pointer & pointer_length_mask) + 1;
            for(pointer_offset = coding_pos - pointer_pos; pointer_length > 0; pointer_length -= 1)
            {
                output[coding_pos] = output[pointer_offset];
                coding_pos += 1;
                pointer_offset += 1;
            }
        }
        else
        {
            pointer_length = 0;
        }
        output[coding_pos] = input[compressed_pointer];
        compressed_pointer += 1;
    }
    return output;
}

Block* decode_block(uint8_t *bytes)
{
    Block *block = malloc(sizeof(Block));
    memcpy(block->magic, bytes, 4);
    memcpy(&block->block_length, bytes + 5, 4);
    if(bytes[4] == ANSIEDIT_LZ77_COMPRESSION)
    {
        memcpy(&block->length, bytes + 9, 4);
        block->bytes = decompress_ansiedit_block(bytes + 9);
    }
    else
    {
        block->length = block->block_length;
        block->bytes = malloc(block->length);
        memcpy(block->bytes, bytes + 9, block->length);
    }
    return block;
}

void free_block(Block *block)
{
    if(block != NULL)
    {
        free(block->bytes);
        free(block);
    }
}

TextArtFile* load_ansiedit_file(char *filename)
{
    FILE        *file_ptr;
    TextArtFile *file;
    uint8_t     *data;
    uint32_t    bytes_pos;
    Block       *parent, *child;
    file         = malloc(sizeof(TextArtFile));
    file_ptr     = fopen(filename, "r");
    file->sauce  = get_sauce(file_ptr);
    file->length = get_real_file_size(file_ptr, file->sauce);
    data         = malloc(file->length);
    bytes_pos    = 0;
    fread(data, 1, file->length, file_ptr);
    fclose(file_ptr);
    parent = decode_block(data);
    if(memcmp(parent->magic, "ANSi", 4) == 0)
    {
        while(bytes_pos < parent->length) {
            child = decode_block(parent->bytes + bytes_pos);
            if(memcmp(child->magic, "DISP", 4) == 0)
            {
                file->screen = create_screen(CHARACTER_AND_ATTRIBUTE_PAIR);
                file->screen->columns   = *((uint16_t *) child->bytes + 0);
                file->screen->rows      = *((uint16_t *) child->bytes + 1);
                file->screen->non_blink = (child->bytes[4] == 1);
                copy_screen_from_bytes(file->screen, child->bytes + 5);
            }
            else if(memcmp(child->magic, "FONT", 4) == 0)
            {
                file->screen->font = create_font_from_bytes(child->bytes[1], 256, child->bytes + 2);
            }
            else if(memcmp(child->magic, "PALE", 4) == 0)
            {
                file->screen->palette = create_new_palette(ANSIEDIT_PALETTE_LENGTH, PAL_DATA_18BIT);
                memcpy(file->screen->palette->data, child->bytes, (size_t) ANSIEDIT_PALETTE_LENGTH * 3);
                generate_rgb_data(file->screen->palette);
            }
            bytes_pos += child->block_length + 9;
            free_block(child);
        }
    }
    free_block(parent);
    free(data);
    return file;
}