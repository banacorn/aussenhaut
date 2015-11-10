
#include "Cello.h"
#include "violin.h"
#include "parser.h"
#include <string.h>

static void Line_New(var self, var args) {
    struct Line* line = self;
    line->commands = get(args, $I(0));
    line->redirection = get(args, $I(1));
    line->socket = get(args, $I(2));
}


static int Line_Show(var self, var output, int pos) {
    struct Line* line = self;
    return print_to(output, pos,
        "{   commands: %$\n    redirection: %$\n    sockets: %$",
        line -> commands, line -> redirection, line -> socket);
}

var Line = Cello(Line,
  Instance(New,     Line_New, NULL),
  Instance(Show,     Line_Show, NULL)
);

var substring(struct String* input, int from, int to)
{
    if (from > to) {
        return new(String, $S(""));
    } else if (0 > from) {
        return new(String, $S(""));
    } else if (to > len(input)) {
        return new(String, $S(""));
    } else {
        char * buffer = malloc(to - from);
        strncpy(buffer, c_str(input) + from, to - from);
        var result = new(String, $S(buffer));
        free(buffer);
        return result;
    }
}

var tokenize(struct String* input, struct String* sep)
{
    var result = new(List, String);
    if (len(input) < len(sep)) {
        return result;
    } else {
        int cursor_old = 0;
        int cursor = 0;
        while ((cursor + len(sep)) < (len(input) + 1)) {
            var sub = substring(input, cursor, cursor + len(sep));
            if (eq(sub, sep)) {
                var cut = substring(input, cursor_old, cursor);
                push(result, cut);
                cursor_old = cursor + len(sep);
            }
            cursor++;
        }
        var rest = substring(input, cursor_old, len(input));
        push(result, rest);
        return result;
    }
}

var not_empty_string(var input)
{
    return len(input) > 0 ? $I(0) : NULL;
}

var compact(struct String* input)
{
    var result = new(List, String);
    foreach (token in filter(tokenize(input, $S(" ")), $(Function, not_empty_string))) {
        push(result, token);
    }
    return result;
}

var parse_command(struct String* input)
{
    return compact(input);
}

var parse_redirection(struct List* tokens)
{
    if (len(tokens) == 0) {
        return NULL;
    }

    bool second_last, redirected = false;
    int i = 0;
    foreach (token in tokens) {
        second_last = i == (len(tokens) - 2);
        redirected = redirected || (eq(token, $S(">")) && second_last);
        i++;
    }

    if (redirected) {
        return get(tokens, $I(len(tokens) - 1));
    } else {
        return NULL;
    }
}

var parse_socket(struct List* tokens)
{
    // if (len(tokens) == 0) {
    //     return NULL;
    // }
    //
    // bool second_last, redirected = false;
    // int i = 0;
    // foreach (token in tokens) {
    //     second_last = i == (len(tokens) - 2);
    //     redirected = redirected || (eq(token, $S(">")) && second_last);
    //     i++;
    // }
    //
    // if (redirected) {
    //     return get(tokens, $I(len(tokens) - 1));
    // } else {
    //     return NULL;
    // }
    return NULL;
}

var parse_line(struct String* input)
{
    var tokens = compact(input);

    //
    //  empty line
    //
    if (len(tokens) == 0) {
        return new(Line, new(List, String), new(String, $S("")), new(Socket, NULL, NULL, NULL));
    }

    //
    //  redirection ">"
    //
    var redirection = parse_redirection(tokens);
    if (redirection) {
        return new(Line, new(List, String), redirection, new(Socket, NULL, NULL, NULL));
    }

    //
    //  socket "|N !N"
    //
    // var socket = parse_socket(tokens);
    // if (socket) {
        return new(Line, new(List, String), redirection, new(Socket, NULL, NULL, NULL));
    // }




    // else {
    // return new(Line, new(List, String), new(String, $S("")), socket);
    //
    // }
    //
    //  redirection ">"
    //
    // bool second_last, redirected = false;
    // int i = 0;
    // var redirection;
    // foreach (token in tokens) {
    //     second_last = i == (len(tokens) - 2);
    //     redirected = redirected || (eq(token, $S(">")) && second_last);
    //     i++;
    // }

    // var socket = new(Socket, NULL, NULL, NULL);
    // var commands = new(List, String);
    // return new(Line, commands, redirection, socket);
}
