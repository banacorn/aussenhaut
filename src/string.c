#include "string.h"
#include "type.h"

#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////


String * string_n(char * chars, int len)
{
    String * new_str = malloc(sizeof(String));
    new_str -> content = malloc(len + 1);
    char * buf = new_str -> content;
    strncpy(buf, chars, len);
    buf[len] = 0;
    return new_str;
}

String * string(char * chars)
{
    return string_n(chars, strlen(chars));
}

String * copy_string(String * str)
{
    return string(str -> content);
}

String * trim(String * str)
{
    char * ref = str -> content;

    // Trim leading space
    while(isspace(*ref)) ref++;

    // All spaces?
    if(*ref == 0) {
        free_string(str);
        return string("");
    } else {
        // Trim trailing space
        char * end = ref + strlen(ref) - 1;
        while(end > ref && isspace(*end)) end--;

        // Write new null terminator
        *(end+1) = 0;
        String * new_str = string(ref);
        free_string(str);
        return new_str;
    }
}

String * substring(String * str, int from, int to)
{
    if (from > to) {
        return string("");
    } else if (0 > from) {
        return string("");
    } else if (to > string_length(str)) {
        return string("");
    } else {
        char * ref = str -> content;
        return string_n(ref + from, to - from);
    }
}

size_t string_length(String * str)
{
    return strlen(str -> content);
}
size_t string_size(String * str)
{
    return strlen(str -> content) + 1;
}

Bool null_string(String * str)
{
    if (strlen(str -> content) == 0)
        return TRUE;
    else
        return FALSE;
}

char * show_string(String * str)
{
    return str -> content;
}

void print_string(String * str)
{
    char * ref = str -> content;
    printf("%s\n", ref);
}

void free_string(String * str)
{
    free(str -> content);
    free(str);
}

Box * box_str(char * chars)
{
    return box(string(chars), (void (*)(void *))free_string, (void * (*)(void *))copy_string, (void (*)(void *))print_string);
}
