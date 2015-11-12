#ifndef PARSER_H_
#define PARSER_H_

#include "Cello.h"
#include "violin.h"
#include "network.h"

struct Line {
    struct List* commands;
    struct String* redirection;     //  '> somewhere'
    struct Socket* socket;
    int ignore;
};
extern var Line;

var trim(struct String*);
var substring(struct String*, int, int);
var tokenize(struct String*, struct String*);
var compact(struct String*);

var parse_command(struct String*);
var parse_line(struct String*);

#endif // PARSER_H_
