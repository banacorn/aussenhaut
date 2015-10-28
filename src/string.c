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

String * take_string(String * xs, int n)
{
    return substring(xs, 0, n);
}

String * drop_string(String * xs, int n)
{
    return substring(xs, n, string_length(xs));
}

Bool compare_string(String * a, String * b)
{
    return (Bool)(strcmp(a -> content, b -> content) == 0);
}

int to_int(String * str)
{
    int result = atoi(str -> content);
    free_string(str);
    return result;
}

Bool numeral(String * str)
{
    if (null_string(str)) {
        return FALSE;
    } else {
        Bool result = TRUE;
        int i = 0;
        char * ref = str -> content;
        while (i < string_length(str)) {
            result = result && isdigit(ref[i]);
            i++;
        }
        return result;
    }
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

Pair * split(String * str, String * sep)
{
    if (string_length(str) < string_length(sep)) {
        Pair * result = pair(box_chars(""), box_str(str));
        free_string(sep);
        return result;
    } else {
        Pair * result;
        int cursor = 0;
        while ((cursor + string_length(sep)) < (string_length(str) + 1)) {
            String * sub = substring(str, cursor, cursor + string_length(sep));

            if (compare_string(sub, sep)) {
                free_string(sub);

                String * first_part = substring(str, 0, cursor);
                String * second_part = substring(str, cursor + string_length(sep), string_length(str));
                free_string(str);
                free_string(sep);
                return pair(box_str(first_part), box_str(second_part));
                break;
            } else {
                free_string(sub);
            }
            cursor++;
        }
        result = pair(box_chars(""), box_str(str));
        free_string(sep);
        return result;
    }
}

Pair * rsplit(String * str, String * sep)
{
    if (string_length(str) < string_length(sep)) {
        Pair * result = pair(box_chars(""), box_str(str));
        free_string(sep);
        return result;
    } else {
        Pair * result;
        int cursor = string_length(str) - string_length(sep);
        while (cursor > 0) {
            String * sub = substring(str, cursor, cursor + string_length(sep));

            if (compare_string(sub, sep)) {
                free_string(sub);

                String * first_part = substring(str, 0, cursor);
                String * second_part = substring(str, cursor + string_length(sep), string_length(str));
                free_string(str);
                free_string(sep);
                return pair(box_str(first_part), box_str(second_part));
                break;
            } else {
                free_string(sub);
            }
            cursor--;
        }
        result = pair(box_chars(""), box_str(str));
        free_string(sep);
        return result;
    }
}

List * tokenize(String * str, String * sep)
{
    if (string_length(str) < string_length(sep)) {
        free_string(sep);
        return cons(box_str(str), nil());
    } else {
        List * result = nil();
        int cursor_old = 0;
        int cursor = 0;
        while ((cursor + string_length(sep)) < (string_length(str) + 1)) {
            String * sub = substring(str, cursor, cursor + string_length(sep));

            if (compare_string(sub, sep)) {
                free_string(sub);
                String * cut = substring(str, cursor_old, cursor);
                result = snoc(result, box_str(cut));
                cursor_old = cursor + string_length(sep);
            } else {
                free_string(sub);
            }
            cursor++;
        }
        String * rest = substring(str, cursor_old, string_length(str));
        result = snoc(result, box_str(rest));
        free_string(str);
        free_string(sep);
        return result;
    }
}

Bool non_null_string_boxed(Box * b)
{
    String * str = unbox(copy(b));
    Bool result = !(null_string(str));
    free_string(str);
    return result;
}

List * compact(List * xs)
{
    return filter(non_null_string_boxed, xs);
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
    printf("\"%s\"", ref);
}

void free_string(String * str)
{
    free(str -> content);
    free(str);
}

Box * box_str(String * str)
{
    return box(str, (void (*)(void *))free_string, (void * (*)(void *))copy_string, (void (*)(void *))print_string);
}

Box * box_chars(char * chars)
{
    return box(string(chars), (void (*)(void *))free_string, (void * (*)(void *))copy_string, (void (*)(void *))print_string);
}
