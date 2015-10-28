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

String * string(char * chars);
String * string_n(char * chars, int len);
String * copy_string(String * str);
String * trim(String * str);
String * substring(String * str, int, int); // allocates new spaces
size_t string_length(String * str);
size_t string_size(String * str);   // memory size
Bool null_string(String * str);
char * show_string(String * str);
void print_string(String * str);
void free_string(String * str);
Box * box_str(char *);

#endif // STRING_H_
