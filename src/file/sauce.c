#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "sauce.h"

Sauce* get_sauce(FILE *file_ptr)
{
    Sauce *sauce = NULL;
    long current = ftell(file_ptr);
    long size;
    char eof, header[5], version[2];
    uint8_t flags;
    fseek(file_ptr, 0L, SEEK_END);
    size = ftell(file_ptr);
    if(size > 129L)
    {
        fseek(file_ptr, -129L, SEEK_END);
        fread(&eof, 1, 1, file_ptr);
        fread(header, 1, 5, file_ptr);
        if(memcmp("SAUCE", header, 5) == 0)
        {
            sauce = malloc(sizeof(Sauce));
            fread(version,           1,  2, file_ptr);
            fread(sauce->title,      1, 35, file_ptr);
            fread(sauce->author,     1, 20, file_ptr);
            fread(sauce->group,      1, 20, file_ptr);
            fread(sauce->date,       1,  8, file_ptr);
            fread(&sauce->file_size, 4,  1, file_ptr);
            fread(&sauce->data_type, 1,  1, file_ptr);
            fread(&sauce->file_type, 1,  1, file_ptr);
            fread(&sauce->t_info_1,  2,  1, file_ptr);
            fread(&sauce->t_info_2,  2,  1, file_ptr);
            fread(&sauce->t_info_3,  2,  1, file_ptr);
            fread(&sauce->t_info_4,  2,  1, file_ptr);
            fread(&sauce->comments,  1,  1, file_ptr);
            fread(&flags,            1,  1, file_ptr);
            sauce->non_blink =      (flags & 1) == 1;
            sauce->letter_spacing = (flags >> 1) & 3;
            sauce->aspect_ratio =   (flags >> 3) & 3;
            fread(sauce->t_info_s,   1, 22, file_ptr);
        }
    }
    fseek(file_ptr, current, SEEK_SET);
    return sauce;
}

void print_sauce_string(char *sauce_string, size_t size)
{
    char string[size + 1];
    memcpy(string, sauce_string, size);
    string[size] = 0;
    printf("%s\n", string);
}

size_t length_of_sauce_string(char *text, size_t max_length)
{
    int32_t last_pos = -1;
    for(size_t i = 0; i < max_length; i += 1)
    {
        if(text[i] != ' ' && text[i] != 0)
        {
            last_pos = i;
        }
    }
    return last_pos + 1;
}

char* get_title(Sauce *sauce)
{
    char *string;
    size_t length_of_title, length_of_author, string_length;
    if(sauce == NULL)
    {
        return NULL;
    }
    length_of_title = length_of_sauce_string(sauce->title, 35);
    length_of_author = length_of_sauce_string(sauce->author, 20);
    if(length_of_title == 0 || length_of_author == 0)
    {
        return NULL;
    }
    string_length = length_of_title + length_of_author + 4;
    string = malloc(string_length + 1);
    memcpy(string, sauce->title, length_of_title);
    memcpy(string + length_of_title, " by ", 4);
    memcpy(string + length_of_title + 4, sauce->author, length_of_author);
    string[string_length] = 0;
    return string;
}

void debug_sauce(Sauce *sauce)
{
    printf("Sauce title: ");
    print_sauce_string(sauce->title,  sizeof(sauce->title));
    printf("Sauce author: ");
    print_sauce_string(sauce->author, sizeof(sauce->author));
    printf("Sauce group: ");
    print_sauce_string(sauce->group,  sizeof(sauce->group));
    printf("Sauce date: ");
    print_sauce_string(sauce->date,   sizeof(sauce->date));
    printf("Sauce file size: %d\n", sauce->file_size);
    printf("Sauce data type: %d\n", sauce->data_type);
    printf("Sauce file type: %d\n", sauce->file_type);
    printf("Sauce TInfo1: %d\n",    sauce->t_info_1);
    printf("Sauce TInfo2: %d\n",    sauce->t_info_2);
    printf("Sauce TInfo3: %d\n",    sauce->t_info_3);
    printf("Sauce TInfo4: %d\n",    sauce->t_info_4);
    printf("Sauce comments: %d\n",  sauce->comments);
    printf("Sauce non-blink (iCE color) mode: ");
    if(sauce->non_blink)
    {
        printf("Yes\n");
    }
    else
    {
        printf("No\n");
    }
    printf("Sauce letter spacing: ");
    switch(sauce->letter_spacing)
    {
        case NO_LETTER_SPACE_PREFERENCE:
        printf("No preference\n");
        break;
        case EIGHT_PIXEL:
        printf("8 pixel\n");
        break;
        case NINE_PIXEL:
        printf("9 pixel\n");
        break;
        case INVALID_LETTER_SPACE:
        printf("Invalid value\n");
        break;
    }
    printf("Sauce aspect ratio: ");
    switch(sauce->aspect_ratio)
    {
        case NO_ASPECT_RATIO_PREFERENCE:
        printf("No preference\n");
        break;
        case LEGACY:
        printf("Legacy device\n");
        break;
        case MODERN:
        printf("Modern device\n");
        break;
        case INVALID_ASPECT_RATIO:
        printf("Invalid value\n");
        break;
    }
    printf("Sauce TInfoS: ");
    print_sauce_string(sauce->t_info_s, sizeof(sauce->t_info_s));
}

uint32_t get_real_file_size(FILE *file_ptr, Sauce *sauce)
{
    long current;
    uint32_t size;
    current = ftell(file_ptr);
    fseek(file_ptr, current, SEEK_END);
    size = (uint32_t) ftell(file_ptr);
    fseek(file_ptr, current, SEEK_SET);
    if(sauce == NULL)
    {
        return size;
    }
    if(sauce->comments == 0)
    {
        return size - 129;
    }
    return size - (5 + sauce->comments * 64 + 129);
}

uint16_t t_info_1(Sauce *sauce, uint16_t default_value)
{
    if(sauce != NULL)
    {
        if(sauce->t_info_1 > 0)
        {
            return sauce->t_info_1;
        }
    }
    return default_value;
}