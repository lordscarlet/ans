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
            fread(sauce->date,       1, 20, file_ptr);
            fread(&sauce->file_size, 4,  1, file_ptr);
            fread(&sauce->data_type, 1,  1, file_ptr);
            fread(&sauce->file_type, 1,  1, file_ptr);
            fread(&sauce->t_info_1,  2,  1, file_ptr);
            fread(&sauce->t_info_2,  2,  1, file_ptr);
            fread(&sauce->t_info_3,  2,  1, file_ptr);
            fread(&sauce->t_info_4,  2,  1, file_ptr);
            fread(&sauce->comments,  1,  1, file_ptr);
            fread(&sauce->t_flags,   1,  1, file_ptr);
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
    printf("Sauce flags: %d\n",     sauce->t_flags);
    printf("Sauce TInfoS: ");
    print_sauce_string(sauce->t_info_s, sizeof(sauce->t_info_s));
}

uint32_t get_actual_file_size(FILE *file_ptr, Sauce *sauce)
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
