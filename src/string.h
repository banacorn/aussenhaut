#ifndef STRING_H_
#define STRING_H_

#include <string.h>
#include "type.h"

////////////////////////////////////////////////////////////////////////////////
//  String
////////////////////////////////////////////////////////////////////////////////

typedef struct String {
    char * content;
} String;

String * string(char *);
String * string_n(char *, int);
String * copy_string(String *);
String * take_string(String *, int);
String * drop_string(String *, int);
Bool compare_string(String *, String *);
int to_int(String *);
Bool numeral(String *);
String * trim(String *);
Pair * split(String *, String *);
Pair * rsplit(String *, String *);  // from the back
List * tokenize(String *, String *);
List * compact(List *);
List * map_string(String *(*)(String *), List *);
String * append_string(String *, String *);
String * concat_string(List *);
String * intercalate_string(List *, String *);
String * substring(String * str, int, int); // allocates new spaces
size_t string_length(String * str);
size_t string_size(String * str);   // memory size
Bool null_string(String * str);
Bool non_null_string(String * str);
char * show_string(String * str);
void print_string(String * str);
void free_string(String * str);

Box * box_str(String *);
Box * box_chars(char *);

#endif // STRING_H_
