#include "string.h"
#include "type.h"

#include <ctype.h>

////////////////////////////////////////////////////////////////////////////////
//  String_
////////////////////////////////////////////////////////////////////////////////


String_ * string_n(char * chars, int len)
{
    String_ * new_str = malloc(sizeof(String_));
    new_str -> content = malloc(len + 1);
    char * buf = new_str -> content;
    strncpy(buf, chars, len);
    buf[len] = 0;
    return new_str;
}

String_ * string(char * chars)
{
    return string_n(chars, strlen(chars));
}

String_ * copy_string(String_ * str)
{
    return string(str -> content);
}

String_ * take_string(String_ * xs, int n)
{
    return substring(xs, 0, n);
}

String_ * drop_string(String_ * xs, int n)
{
    return substring(xs, n, string_length(xs));
}

Bool compare_string(String_ * a, String_ * b)
{
    return (Bool)(strcmp(a -> content, b -> content) == 0);
}

int to_int(String_ * str)
{
    int result = atoi(str -> content);
    free_string(str);
    return result;
}

Bool numeral(String_ * str)
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

String_ * trim(String_ * str)
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
        String_ * new_str = string(ref);
        free_string(str);
        return new_str;
    }
}

Pair * split(String_ * str, String_ * sep)
{
    if (string_length(str) < string_length(sep)) {
        Pair * result = pair(box_chars(""), box_str(str));
        free_string(sep);
        return result;
    } else {
        Pair * result;
        int cursor = 0;
        while ((cursor + string_length(sep)) < (string_length(str) + 1)) {
            String_ * sub = substring(str, cursor, cursor + string_length(sep));

            if (compare_string(sub, sep)) {
                free_string(sub);

                String_ * first_part = substring(str, 0, cursor);
                String_ * second_part = substring(str, cursor + string_length(sep), string_length(str));
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

Pair * rsplit(String_ * str, String_ * sep)
{
    if (string_length(str) < string_length(sep)) {
        Pair * result = pair(box_chars(""), box_str(str));
        free_string(sep);
        return result;
    } else {
        Pair * result;
        int cursor = string_length(str) - string_length(sep);
        while (cursor > 0) {
            String_ * sub = substring(str, cursor, cursor + string_length(sep));

            if (compare_string(sub, sep)) {
                free_string(sub);

                String_ * first_part = substring(str, 0, cursor);
                String_ * second_part = substring(str, cursor + string_length(sep), string_length(str));
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

List_ * tokenize(String_ * str, String_ * sep)
{
    if (string_length(str) < string_length(sep)) {
        free_string(sep);
        return cons(box_str(str), nil());
    } else {
        List_ * result = nil();
        int cursor_old = 0;
        int cursor = 0;
        while ((cursor + string_length(sep)) < (string_length(str) + 1)) {
            String_ * sub = substring(str, cursor, cursor + string_length(sep));

            if (compare_string(sub, sep)) {
                free_string(sub);
                String_ * cut = substring(str, cursor_old, cursor);
                result = snoc(result, box_str(cut));
                cursor_old = cursor + string_length(sep);
            } else {
                free_string(sub);
            }
            cursor++;
        }
        String_ * rest = substring(str, cursor_old, string_length(str));
        result = snoc(result, box_str(rest));
        free_string(str);
        free_string(sep);
        return result;
    }
}

Bool non_null_string_boxed(Box_ * b)
{
    String_ * str = unbox(copy_(b));
    Bool result = !(null_string(str));
    free_string(str);
    return result;
}

List_ * compact(List_ * xs)
{
    return filter_(non_null_string_boxed, xs);
}

List_ * map_string(String_ *(*f)(String_ *), List_ * xs)
{
    if (xs -> Nil) {
        return xs;
    } else {
        List_ * result = map_string(f, xs -> Cons);
        String_ * val = f(unbox(xs -> data));
        free(xs);
        return cons(box_str(val), result);
    }
}

String_ * append_string(String_ * xs, String_ * ys)
{
    // allocate space
    String_ * result = malloc(sizeof(String_));
    result -> content = malloc(string_length(xs) + string_length(ys) + 1);

    // copy
    char * xs_ref = xs -> content;
    char * ys_ref = ys -> content;
    char * result_ref = result -> content;
    memcpy(result_ref, xs_ref, string_size(xs));
    memcpy(result_ref + string_length(xs), ys_ref, string_size(ys));
    result_ref[string_length(xs) + string_length(ys)] = 0;

    // cleanup
    free_string(xs);
    free_string(ys);

    return result;
}

String_ * concat_string(List_ * xs)
{
    if (xs -> Nil) {
        free_list(xs);
        return string("");
    } else {
        String_ * result = concat_string(xs -> Cons);
        result = append_string(unbox(xs -> data), result);
        free(xs);
        return result;
    }
}

String_ * intercalate_string(List_ * xs, String_ * sep)
{
    if (xs -> Nil) {
        free_list(xs);
        return string("");
    } else if (xs -> Cons -> Nil) {
        String_ * result = unbox(copy_(xs -> data));
        free_list(xs);
        free_string(sep);
        return result;
    } else {
        String_ * result = intercalate_string(xs -> Cons, copy_string(sep));
        result = append_string(sep, result);
        result = append_string(unbox(xs -> data), result);
        free(xs);
        return result;
    }
}

String_ * substring(String_ * str, int from, int to)
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

size_t string_length(String_ * str)
{
    return strlen(str -> content);
}
size_t string_size(String_ * str)
{
    return strlen(str -> content) + 1;
}

Bool null_string(String_ * str)
{
    if (strlen(str -> content) == 0)
        return TRUE;
    else
        return FALSE;
}

char * show_string(String_ * str)
{
    return str -> content;
}

void print_string(String_ * str)
{
    char * ref = str -> content;
    printf("\"%s\"", ref);
}

void free_string(String_ * str)
{
    free(str -> content);
    free(str);
}

Box_ * box_str(String_ * str)
{
    return box(str, (void (*)(void *))free_string, (void * (*)(void *))copy_string, (void (*)(void *))print_string);
}

Box_ * box_chars(char * chars)
{
    return box(string(chars), (void (*)(void *))free_string, (void * (*)(void *))copy_string, (void (*)(void *))print_string);
}
