#include "util.h"
#include "type.h"

////////////////////////////////////////////////////////////////////////////////
//  Command
////////////////////////////////////////////////////////////////////////////////

Command * command(String * name, List * args)
{
    Command * node = malloc(sizeof(Command));
    node -> name = name;
    node -> args = args;
    return node;
}

Command * copy_command(Command * node)
{
    return command(copy_string(node -> name), copy_list(node -> args));
}

Command * parse_command(String * str)
{
    str = trim(str);
    if (null_string(str)) {
        free_string(str);
        return command(string(""), nil());
    } else {
        List * tokens = compact(tokenize(str , string(" ")));
        String * name = head(tokens);
        List * args = tail(tokens);
        free_list(tokens);
        return command(name, args);
    }
}

int arg_length(Command * node)
{
    return length(node -> args);
}

void free_command(Command * node)
{
    free_string(node -> name);
    free_list(node -> args);
    free(node);
}


void print_command(Command * node)
{
    if (node -> args -> Nil) {
        printf("%s", node -> name -> content);
    } else {
        printf("%s ", node -> name -> content);
        print_list(node -> args);
    }
}

Box * box_cmd(Command * cmd)
{
    return box(cmd, (void (*)(void *))free_command, (void * (*)(void *))copy_command, (void (*)(void *))print_command);
}

////////////////////////////////////////////////////////////////////////////////
//  Line
////////////////////////////////////////////////////////////////////////////////

Line * line(List * cmds, Bool redirect, String * target, int out, int err)
{
    Line * node = malloc(sizeof(Line));
    node -> cmds = cmds;
    node -> redirect = redirect;
    node -> target = target;
    node -> out = out;
    node -> err = err;
    return node;
}

Box * parse_command_boxed(Box * data)
{
    return box_cmd(parse_command(unbox(data)));
}

// String -> Maybe (List Command, String)
Maybe * parse_redirection(String * str)
{
    str = trim(str);
    Pair * splitted = rsplit(str, string(" > "));
    String * first_part = fst(splitted);
    String * second_part = snd(splitted);

    if (null_string(first_part)) {  // no " > "
        free_string(first_part);
        free_string(second_part);
        free_pair(splitted);
        return nothing();
    } else {                        // has " > "
        List * snd_part_cmds = compact(tokenize(second_part, string(" ")));
        if (length(snd_part_cmds) == 1) {   // OK
            String * last_str = head(snd_part_cmds);
            List * commands = map(parse_command_boxed, tokenize(first_part, string(" | ")));
            free_list(snd_part_cmds);
            free_pair(splitted);
            return just(box_pair(pair(box_list(commands), box_str(last_str))));
        } else {
            free_list(snd_part_cmds);
            free_string(first_part);
            free_pair(splitted);
            return nothing();
        }
    }
}

// String -> Int
int parse_numbered_pipe(String * str)
{
    int result = -1;
    String * head = take_string(str, 1);
    String * tail = drop_string(str, 1);
    String * pipe = string("|");

    if (compare_string(head, pipe) && numeral(tail)) {
        result = to_int(tail);
    } else {
        free_string(tail);
    }
    free_string(head);
    free_string(pipe);
    free_string(str);
    return result;
}

// String -> Int
int parse_numbered_bang(String * str)
{
    int result = -1;
    String * head = take_string(str, 1);
    String * tail = drop_string(str, 1);
    String * bang = string("!");

    if (compare_string(head, bang) && numeral(tail)) {
        result = to_int(tail);
    } else {
        free_string(tail);
    }
    free_string(head);
    free_string(bang);
    free_string(str);
    return result;
}

// String -> Maybe {String, Int, Int}
Maybe * parse_piping(String * str)
{
    Maybe * result = NULL;
    int numbered_pipe = 0;
    int out = -1;
    int err = -1;

    // preprocess
    str = trim(str);
    List * tokens = compact(tokenize(copy_string(str), string(" ")));

    if (length(tokens) >= 2) {
        String * last_str = last(tokens);
        List * temp_init = init(tokens);
        String * snd_last_str = last(temp_init);

        int last_is_pipe = parse_numbered_pipe(copy_string(last_str));
        int last_is_bang = parse_numbered_bang(copy_string(last_str));

        int snd_last_is_pipe = parse_numbered_pipe(copy_string(snd_last_str));
        int snd_last_is_bang = parse_numbered_bang(copy_string(snd_last_str));

        if (snd_last_is_bang > -1 && last_is_pipe > -1) { // !N |N
            printf("!%d |%d\n", snd_last_is_bang, last_is_pipe);
            struct {
                String * str;
                int out;
                int err;
            } package;

            print_list(init(temp_init));
            // package.str = substring(str, 0, )

            // result = just();
            result = nothing();
        } else if (snd_last_is_pipe > -1 && last_is_bang > -1) { // |N !N
            printf("|%d !%d\n", snd_last_is_pipe, last_is_bang);
            result = nothing();
        } else if (last_is_bang > -1) { // !N
            print_list(temp_init);
            printf("!%d\n", last_is_bang);
            result = nothing();
        } else if (last_is_pipe > -1) { // |N
            print_list(temp_init);
            printf("|%d\n", last_is_pipe);
            result = nothing();
            // res
        } else  { // nothing
            print_list(tokens);
            printf("NOPE\n");
            result = nothing();
        }


        free_list(temp_init);
        free_string(last_str);
        free_string(snd_last_str);

    } else {
        result = nothing();
    }

    print_list(tokens);
    free_list(tokens);

    free_string(str);
    return result;
}


Line * parse_line(String * str)
{
    int numbered_pipe = 0;
    Bool redirect = FALSE;
    int out = -1;
    int err = -1;
    str = trim(str);
    printf("=== input string ===\n");
    print_string(str);
    printf("\n");

    Maybe * result_redirection = parse_redirection(copy_string(str));
    if (result_redirection -> Nothing == FALSE) {    // redirected
        Pair * splitted = from_just(result_redirection);
        Line * result = line(fst(splitted), TRUE, snd(splitted), -1, -1);

        // cleanup
        free_pair(splitted);
        free_maybe(result_redirection);
        free_string(str);
        return result;
    } else {

        printf("=== parsed piping shit ===\n");
        Maybe * result_piping = parse_piping(str);
        print_maybe(result_piping);
        printf("\n");


        // cleanup
        free_maybe(result_piping);
        free_maybe(result_redirection);

        return line(nil(), redirect, string(""), out, err);
    }
}



//     String * str = trim(raw_str);
//     ListCmd * cmds = nil_cmd();
//     int numbered_pipe = 0;
//     Bool redirect = FALSE;
//     int out = -1;
//     int err = -1;
//
//     if (string_length(str) == 0) {
//         free_string(str);
//         return line(cmds, redirect, out, err);
//     } else if (string_length(str) == 1) {
//         cmds = cons_cmd(parse_command(str), cmds);
//         return line(cmds, redirect, out, err);
//     }
//
//     // has numbered pipe at the end?
//     char * end = str -> content + string_length(str) - 1;
//     if (isdigit(*end)) {
//         while (end > str -> content && isdigit(*end)) end--;
//         if (*(end - 1) == ' ' && *end == '|') {
//             out = atoi(end + 1);
//             numbered_pipe++;
//         } else if (*(end - 1) == ' ' && *end == '!') {
//             err = atoi(end + 1);
//             numbered_pipe++;
//         }
//     }
//
//
//     // upon discovery, fill the parsed part with spaces
//     if (numbered_pipe == 1) {
//         while(end < str -> content + string_length(str)) {
//             *end = ' ';
//             end++;
//         }
//         str = trim(str);
//     }
//
//     // has numbered pipe at the end?
//     end = str -> content + string_length(str) - 1;
//     if (isdigit(*end)) {
//         while (end > str -> content && isdigit(*end)) end--;
//         if (*(end - 1) == ' ' && *end == '|' && out == -1) {
//             out = atoi(end + 1);
//             numbered_pipe++;
//         } else if (*(end - 1) == ' ' && *end == '!' && err == -1) {
//             err = atoi(end + 1);
//             numbered_pipe++;
//         }
//     }
//
//     // upon discovery, fill the parsed part with spaces
//     if (numbered_pipe == 2) {
//         while(end < str -> content + string_length(str)) {
//             *end = ' ';
//             end++;
//         }
//         str = trim(str);
//     }
//
//     // seperating input with " | " and cons the substring before it
//     int offset = 0;
//     int i = 0;
//     char * ref = str -> content;
//     while (i + 2 < string_length(str)) {
//         if (ref[i] == ' ' && ref[i + 1] == '|' && ref[i + 2] == ' ') {
//             // found!
//             Command * cmd = parse_command(substring(str, offset, i));
//             cmds = snoc_cmd(cmds, cmd);
//             offset = i = i + 3;
//         } else {
//             i++;
//         }
//     }
//
//     // see if there's redirection ">" in the last 2 commands
//     Command * cmd = parse_command(substring(str, offset, i + 2));
//     cmds = snoc_cmd(cmds, cmd);
//     free_string(str);
//     return line(cmds, redirect, out, err);
// }
//
Line * copy_line(Line * node)
{
    return line(copy_list(node -> cmds), node -> redirect, node -> target, node -> out, node -> err);
}

void print_line(Line * node)
{
    if (node -> redirect) {         // has ">"
        print_list(node -> cmds);
        printf(" > ");
        print_string(node -> target);
    } else {
        print_list(node -> cmds);
        if (!null(node -> cmds)) {
            if (node -> out != -1)
                printf(" |%d", node -> out);
            if (node -> err != -1)
                printf(" !%d", node -> err);
        }
    }
    printf("\n");
}

void free_line(Line * node)
{
    free_list(node -> cmds);
    free_string(node -> target);
    free(node);
}
