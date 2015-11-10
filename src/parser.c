
#include "Cello.h"
#include "violin.h"
#include "parser.h"
#include <string.h>
#include <stdlib.h>

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
var parse_body(struct List* tokens)
{
    var result = new(List, List);
    var command = new(List, String);
    foreach (token in tokens) {
        if (eq(token, $S("|"))) {
            push(result, command);
            command = new(List, String);
        } else {
            push(command, token);
        }
    }
    push(result, command);  // the rest

    return result;
}

var parse_pipe(struct String* input)
{
    char *ref = c_str(input);
    if (ref[0] == '|') {
        return new(Int, $I(atoi(ref + 1)));
    } else {
        return NULL;
    }
}

var parse_bang(struct String* input)
{
    char *ref = c_str(input);
    if (ref[0] == '!') {
        return new(Int, $I(atoi(ref + 1)));
    } else {
        return NULL;
    }
}

var parse_socket(struct List* tokens)
{
    int tokens_length = len(tokens);
    if (tokens_length == 0) {
        return NULL;
    }

    // last
    var last_token = get(tokens, $I(tokens_length - 1));
    var sout = parse_pipe(last_token);
    var serr = parse_bang(last_token);
    if (sout) {
        if (tokens_length >= 2) {
            var last_second = get(tokens, $I(tokens_length - 2));
            serr = parse_bang(last_second);
        }

    } else if (serr) {
        if (tokens_length >= 2) {
            var last_second = get(tokens, $I(tokens_length - 2));
            sout = parse_pipe(last_second);
        }
    }
    if (sout && serr)
        return new(Socket, $I(-1), sout, serr);
    if (sout) {
        return new(Socket, $I(-1), sout, $I(-1));
    }
    if (serr)
        return new(Socket, $I(-1), $I(-1), serr);

    return new(Socket, $I(-1), $I(-1), $I(-1));
}

var parse_line(struct String* input)
{
    var tokens = compact(input);
    int tokens_length = len(tokens);

    //
    //  empty line
    //
    if (tokens_length == 0) {
        return new(Line, new(List, List), new(String, $S("")), new(Socket, $I(-1), $I(-1), $I(-1)));
    }

    //
    //  redirection ">"
    //
    var redirection = parse_redirection(tokens);
    if (redirection) {
        var body = parse_body(take(tokens, tokens_length - 2));
        return new(Line, body, redirection, new(Socket, $I(-1), $I(-1), $I(-1)));
    }

    //
    //  socket "|N !N"
    //
    struct Socket* socket = parse_socket(tokens);
    int count = 0;  // count how much to be parsed as command body
    if (socket->sout != -1) count++;
    if (socket->serr != -1) count++;
    var body = parse_body(take(tokens, tokens_length - count));
    return new(Line, body, new(String, $S("")), socket);

}
