#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "text/text.h"
#include "other/ans24.h"

char** grab_filenames(char const **args, uint32_t filenames_length)
{
    char **filenames = malloc(sizeof(char*) * filenames_length);
    size_t string_length;
    for(uint32_t i = 0; i < filenames_length; i += 1)
    {
        string_length = strlen(args[i]) + 1;
        filenames[i] = malloc(sizeof(char) * string_length);
        strcpy(filenames[i], args[i]);
    }
    return filenames;
}

void free_filenames(char **filenames, uint32_t filenames_length)
{
    if(filenames != NULL)
    {
        for(uint32_t i = 0; i < filenames_length; i += 1)
        {
            free(filenames[i]);
        }
        free(filenames);
    }
}

int main(int argc, char const *argv[])
{
    uint32_t filenames_length;
    char **filenames;
    filenames_length = (uint32_t) argc - 1;
    filenames = grab_filenames(argv + 1, filenames_length);
    if(argc == 1)
    {
        printf("Usage: anscat [file ...]\n");
        return 0;
    }
    // display_window(filenames, filenames_length, true);
    ans24_main(filenames, filenames_length);
    free_filenames(filenames, filenames_length);
    return 0;
}
