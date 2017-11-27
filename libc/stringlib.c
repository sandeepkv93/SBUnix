#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stringlib.h>
#include <stringll.h>

#define TOKEN_SIZE 1024

void
lib_str_clean(char* var)
{
    int i;
    for (i = 0; i < TOKEN_SIZE; ++i) {
        var[i] = '\0';
    }
}

int
lib_str_find(char* str1, char* str2)
{
    int i = 0;
    char *x, *y;
    while (*str1 != '\0') {
        x = str1;
        y = str2;
        while (*y == *x) {
            x++;
            y++;
            if (*y == '\0') {
                return i;
            }
        }
        i++;
        str1++;
    }
    return -1;
}

void
lib_str_remove_extra_spaces(char* input_string)
{
    int initial_string_length = strlen(input_string);
    int dst = 0;
    int src = 0;
    while (src < initial_string_length && input_string[src] == ' ') {
        ++src;
    }
    while (src < initial_string_length) {

        if (input_string[src] == ' ') {

            input_string[dst] = input_string[src];
            ++dst;
            ++src;

            while (src < initial_string_length && input_string[src] == ' ') {
                ++src;
            }

        } else if (input_string[src] == '!' || input_string[src] == '?') {
            if (input_string[dst - 1] == ' ') {
                input_string[dst - 1] = input_string[src];
                src++;
            } else {
                input_string[dst++] = input_string[src++];
            }

        } else {
            input_string[dst++] = input_string[src++];
        }
    }

    if (dst <= src) {
        --dst;
        if (input_string[dst] == ' ') {
            input_string[dst] = '\0';
        }
        ++dst;
        while (dst < initial_string_length) {
            input_string[dst++] = '\0';
        }
    }
}

int
lib_str_split(const char* string_, char delimiter, struct stringllnode** start)
{
    int len = strlen(string_);
    char* st = (char*)malloc(sizeof(char) * len);
    strcpy(st, string_);
    char token[TOKEN_SIZE];
    int beg_index = 0;
    int end_index = 0;
    int i;
    for (i = 0; string_[i] != '\0'; ++i) {
        if (string_[i] == delimiter) {
            lib_str_clean(token);
            // TODO: Does the string need termination? Please check
            strncpy(token, string_ + beg_index, (end_index - beg_index));
            beg_index = end_index + 1;
            lib_str_remove_extra_spaces(token);
            append(start, token);
        }
        ++end_index;
    }
    lib_str_clean(token);
    // TODO: Does the string need termination? Please check
    strncpy(token, string_ + beg_index, (end_index - beg_index));
    lib_str_remove_extra_spaces(token);
    append(start, token);
    return 0;
}

int
lib_str_split_get_member(const char* string_, char delimiter, int index,
                         char* a)
{
    int t;
    struct stringllnode* head = NULL;
    lib_str_split(string_, delimiter, &head);
    t = get_node(head, index, a);
    free_list(head);
    return t;
}
