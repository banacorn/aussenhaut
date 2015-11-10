#ifndef STRING_H_
#define STRING_H_

#include <string.h>
#include "type.h"

////////////////////////////////////////////////////////////////////////////////
//  String_
////////////////////////////////////////////////////////////////////////////////

typedef struct String_ {
    char * content;
} String_;

String_ * string(char *);
String_ * string_n(char *, int);
String_ * copy_string(String_ *);
String_ * take_string(String_ *, int);
String_ * drop_string(String_ *, int);
Bool compare_string(String_ *, String_ *);
int to_int(String_ *);
Bool numeral(String_ *);
String_ * trim(String_ *);
Pair * split(String_ *, String_ *);
Pair * rsplit(String_ *, String_ *);  // from the back
List_ * tokenize(String_ *, String_ *);
List_ * compact(List_ *);
List_ * map_string(String_ *(*)(String_ *), List_ *);
String_ * append_string(String_ *, String_ *);
String_ * concat_string(List_ *);
String_ * intercalate_string(List_ *, String_ *);
String_ * substring(String_ * str, int, int); // allocates new spaces
size_t string_length(String_ * str);
size_t string_size(String_ * str);   // memory size
Bool null_string(String_ * str);
Bool non_null_string(String_ * str);
char * show_string(String_ * str);
void print_string(String_ * str);
void free_string(String_ * str);

Box_ * box_str(String_ *);
Box_ * box_chars(char *);

#endif // STRING_H_
